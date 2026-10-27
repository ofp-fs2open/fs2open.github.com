// Keeps the Community Queue (discussion #1) and the Feature Queue categories in step
// with the Queue Accepted label and with PRs that reference queued requests.
// Run by .github/workflows/community-queue.yaml; see CONTRIBUTING.md for the policy.
//
// The Active Queue section of discussion #1 is the record of what is queued, in
// acceptance order. Only that section is rewritten; the rest of the post is left alone.

const QUEUE_DISCUSSION = 1;
const QUEUE_LABEL = 'Queue Accepted';
const CATEGORY = {
	requests: 'Feature Requests',
	active: 'Active Requests',
	completed: 'Completed Requests',
	stale: 'Stale Requests',
};

const SECTION_START = '## Active Queue\n';
const SECTION_END = '\n---\n';
const EMPTY_QUEUE = '_The queue is empty._';

// GitHub's issue-closing keywords, in any case, with an optional colon. Only a bare
// #N counts: owner/repo#N refers to another repository.
const KEYWORD_REF = /(?:^|[^\w/#])(?:close[sd]?|fix(?:e[sd])?|resolve[sd]?):?\s+#(\d+)\b/gi;

function parseQueue(body) {
	body = body.replace(/\r\n/g, '\n');
	const start = body.indexOf(SECTION_START);
	if (start < 0)
		throw new Error(`Discussion #${QUEUE_DISCUSSION} has no "## Active Queue" section`);
	const contentStart = start + SECTION_START.length;
	const end = body.indexOf(SECTION_END, contentStart);
	if (end < 0)
		throw new Error(`Discussion #${QUEUE_DISCUSSION}'s Active Queue section has no closing ---`);

	const entries = [];
	const entry = /\*\*#\d+ - \[(.*)\]\((\S*\/discussions\/(\d+))\)\*\*\nAccepted: (.*)\nStatus: (.*)/g;
	let m;
	while ((m = entry.exec(body.slice(contentStart, end))) !== null)
		entries.push({ title: m[1], url: m[2], number: parseInt(m[3], 10), accepted: m[4].trim(), status: m[5].trim() });

	return { before: body.slice(0, contentStart), after: body.slice(end), entries };
}

function renderQueue(queue) {
	const list = queue.entries.length === 0 ? EMPTY_QUEUE : queue.entries.map((e, i) =>
		`**#${i + 1} - [${e.title}](${e.url})**\nAccepted: ${e.accepted}\nStatus: ${e.status}`).join('\n\n');
	return `${queue.before}\n${list}\n${queue.after}`;
}

function referencedNumbers(text) {
	const found = new Set();
	for (const m of (text || '').matchAll(KEYWORD_REF))
		found.add(parseInt(m[1], 10));
	return found;
}

const today = () => new Date().toISOString().slice(0, 10);
const prLink = pr => `[#${pr.number}](${pr.html_url})`;

module.exports = async ({ github, context, core }) => {
	const { owner, repo } = context.repo;
	const gql = (query, vars = {}) => github.graphql(query, vars);

	const repoInfo = (await gql(`query($owner: String!, $repo: String!, $queue: Int!, $label: String!) {
		repository(owner: $owner, name: $repo) {
			discussionCategories(first: 50) { nodes { id name } }
			label(name: $label) { id }
			discussion(number: $queue) { id body }
		}
	}`, { owner, repo, queue: QUEUE_DISCUSSION, label: QUEUE_LABEL })).repository;

	const categoryId = {};
	for (const [key, name] of Object.entries(CATEGORY)) {
		const cat = repoInfo.discussionCategories.nodes.find(c => c.name === name);
		if (!cat)
			throw new Error(`Discussion category "${name}" not found`);
		categoryId[key] = cat.id;
	}
	if (!repoInfo.discussion)
		throw new Error(`Discussion #${QUEUE_DISCUSSION} not found`);

	const queue = parseQueue(repoInfo.discussion.body);
	const originalBody = renderQueue(queue);
	const inQueue = n => queue.entries.find(e => e.number === n);
	const removeFromQueue = n => { queue.entries = queue.entries.filter(e => e.number !== n); };
	const queueUrl = `https://github.com/${owner}/${repo}/discussions/${QUEUE_DISCUSSION}`;

	const getDiscussion = async number => (await gql(`query($owner: String!, $repo: String!, $number: Int!) {
		repository(owner: $owner, name: $repo) {
			discussion(number: $number) { id number title url closed category { id } }
		}
	}`, { owner, repo, number })).repository.discussion;

	const moveTo = async (discussion, key) => {
		if (discussion.category.id === categoryId[key])
			return;
		await gql(`mutation($id: ID!, $cat: ID!) { updateDiscussion(input: {discussionId: $id, categoryId: $cat}) { clientMutationId } }`,
			{ id: discussion.id, cat: categoryId[key] });
		core.info(`Moved #${discussion.number ?? ''} to ${CATEGORY[key]}`);
	};
	const comment = (discussion, body) =>
		gql(`mutation($id: ID!, $body: String!) { addDiscussionComment(input: {discussionId: $id, body: $body}) { clientMutationId } }`,
			{ id: discussion.id, body });

	const event = context.eventName;
	const action = context.payload.action;

	if (event === 'discussion') {
		const number = context.payload.discussion.number;
		if (number === QUEUE_DISCUSSION)
			return;
		const discussion = await getDiscussion(number);
		// Closed and completed requests are done. To work on one again, a maintainer moves
		// and reopens it by hand.
		if (!discussion || discussion.closed || discussion.category.id === categoryId.completed) {
			core.info(`#${number} is closed or completed; leaving it alone`);
			return;
		}

		if (action === 'labeled' && context.payload.label.name === QUEUE_LABEL) {
			if (!inQueue(number)) {
				queue.entries.push({ title: discussion.title, url: discussion.url, number, accepted: today(), status: 'Open' });
				await comment(discussion, `Accepted into the [Community Queue](${queueUrl}) at position ${queue.entries.length}.`);
			}
			await moveTo(discussion, 'active');
		} else if (action === 'unlabeled' && context.payload.label.name === QUEUE_LABEL) {
			removeFromQueue(number);
			if (discussion.category.id === categoryId.active)
				await moveTo(discussion, 'requests');
		} else if (action === 'category_changed' && discussion.category.id === categoryId.stale && inQueue(number)) {
			// Gone stale: out of the queue. Reactivating means accepting it again, at the back.
			removeFromQueue(number);
			if (repoInfo.label)
				await gql(`mutation($id: ID!, $label: ID!) { removeLabelsFromLabelable(input: {labelableId: $id, labelIds: [$label]}) { clientMutationId } }`,
					{ id: discussion.id, label: repoInfo.label.id });
		}
	} else if (event === 'pull_request_target') {
		const pr = context.payload.pull_request;
		if (pr.base.ref !== context.payload.repository.default_branch)
			return;
		const refs = referencedNumbers(pr.body);
		const ownedByPr = e => e.status.includes(`/pull/${pr.number})`);

		if (action === 'closed' && pr.merged) {
			for (const number of refs) {
				if (!inQueue(number))
					continue;
				const discussion = await getDiscussion(number);
				removeFromQueue(number);
				if (!discussion)
					continue;
				await moveTo(discussion, 'completed');
				await comment(discussion, `Implemented by ${prLink(pr)}.`);
				if (!discussion.closed)
					await gql(`mutation($id: ID!) { closeDiscussion(input: {discussionId: $id, reason: RESOLVED}) { clientMutationId } }`,
						{ id: discussion.id });
			}
			// Items this PR had marked In Progress but no longer references go back to Open.
			for (const e of queue.entries)
				if (ownedByPr(e))
					e.status = 'Open';
		} else if (action === 'closed') {
			for (const e of queue.entries)
				if (ownedByPr(e))
					e.status = 'Open';
		} else {
			// opened, reopened, edited
			for (const e of queue.entries) {
				if (refs.has(e.number))
					e.status = `In Progress (${prLink(pr)})`;
				else if (ownedByPr(e))
					e.status = 'Open';
			}
		}
	}

	const newBody = renderQueue(queue);
	if (newBody !== originalBody) {
		await gql(`mutation($id: ID!, $body: String!) { updateDiscussion(input: {discussionId: $id, body: $body}) { clientMutationId } }`,
			{ id: repoInfo.discussion.id, body: newBody });
		core.info('Updated the Community Queue');
	}
};

module.exports.parseQueue = parseQueue;
module.exports.renderQueue = renderQueue;
module.exports.referencedNumbers = referencedNumbers;
