# Contributing to the Open FreeSpace Project

Welcome. OFP is a community-governed fork of FreeSpace Open. This document covers how to contribute code, how the Community Queue works, and what to expect from the review and merge process.

---

## Before You Start

- **Bug fixes** are always welcome. Open a PR, get it merged. No prerequisites.
- **Feature work** should draw from the Community Queue (see below). Personal features are allowed but subject to reviewer prioritization.
- All contributions must be compatible with FSO licensing terms. New OFP-originated contributions are released under the Unlicense.

---

## The Community Queue

Feature requests live in [GitHub Discussions](../../discussions). The [Community Queue](../../discussions/1) is the ordered list of accepted, active feature requests that contributors draw from when choosing what to implement.

The Queue is kept up to date automatically. Maintainers only add or remove the **Queue Accepted** label; don't edit the Active Queue list in the Community Queue post by hand.

### How a request enters the Queue

1. Open a Discussion using the Feature Request template (problem statement, proposed solution, mod/player impact).
2. The community engages - upvotes, comments, refinements.
3. A Reviewer or Maintainer adds the **Queue Accepted** label when it meets the bar: clearly defined, technically feasible, and demonstrably wanted by currently active community members.
4. It enters the back of the Queue, dated the day it was accepted, and moves to the **Active Requests** category.

Removing the label takes it back out of the Queue and returns it to Feature Requests. Closed requests and those in Completed Requests are left alone: to queue one again, a maintainer moves it out of Completed Requests and reopens it first.

### Working the Queue

Draw from the Queue **oldest-accepted first**. If you skip ahead, document why in your PR description. Acceptable reasons are narrow: you lack the technical background for the oldest items, or your expertise is specifically needed elsewhere. Maintainers may push back if skips become a pattern.

Reference the request in your PR description with `Fixes #<discussion>` (or Closes, Resolves, or any other keyword GitHub uses to close issues). While the PR is open, the request shows as In Progress in the Queue, and a comment on the request links the PR so its followers know. When it merges, the request leaves the Queue, moves to **Completed Requests**, and is closed as resolved. If the PR is closed without merging, the request goes back to Open in its original place, with a comment saying so.

### Stale requests

A request can be marked **Stale** if the original author and supporters have been inactive for over a year with no meaningful recent engagement. Moving a request to the **Stale Requests** category takes it out of the Queue and removes its label. Any active community member can ask for it to be reactivated; a maintainer then adds the label again, and it re-enters the Queue at the back.

---

## Code Review Matching

OFP operates on community reciprocity. Review bandwidth is a shared resource. Contributors who consistently participate in reviews or Queue work may be prioritized when Maintainer bandwidth is limited.

Community contribution means any combination of:
- Meaningful code reviews on other contributors' PRs
- Implementing features from the Community Queue

There is no quota. Two Queue implementations and zero reviews is fine. Zero Queue implementations and several substantive reviews is also fine. Maintainers exercise judgment publicly and with documented reasoning.

**Bug fixes are always exempt.**

---

## Contributor Ladder

### Contributor
**Eligibility:** One or more merged PRs.
**How:** Self-nominate or be nominated in a public GitHub Discussion. Approved by Maintainer consensus.
**Authority:** None beyond submitting PRs.

### Reviewer
**Eligibility:** 5 or more merged PRs, plus meaningful participation in at least 3 code reviews on other contributors' work.
**How:** Self-nominate or be nominated in a public GitHub Discussion. Approved by Maintainer consensus.
**Authority:** Can approve PRs. Cannot merge.

### Maintainer
**Eligibility:** Active Reviewer and code contributor for at least 6 months, demonstrated consistent good judgment in reviews, and active participation in RFC discussions.
**How:** Self-nominate or be nominated in a public GitHub Discussion. Approved by Maintainer consensus.
**Authority:** Can merge PRs. Responsible for Queue management and RFC rulings.

Good judgment means: reviews that catch real issues, RFC feedback that is constructive and technically grounded, no pattern of blocking contributions without documented reasoning. The public record is the evidence.

### Removal

Any tier can be lost for inactivity (12 months with no activity, 30-day notice) or through a documented conduct or judgment process. The process is the same as promotion: public, reasoned, and on the record.

### Recusal

A Maintainer or Reviewer with a direct personal stake in a decision recuses themselves and says so publicly. Direct personal stake means: the PR or RFC primarily serves their own mod or project, there is documented ongoing conflict with the contributor involved, or they authored the code under review.

A recused Maintainer may still comment technically but does not vote or merge. If recusal leaves fewer than two Maintainers able to rule, the decision opens to a community vote in the relevant Discussion thread. Any active Contributor or above may vote.

---

## Submitting a PR

1. Fork the repo and create a branch from `master`.
2. Write clear commit messages.
3. If implementing a Queue item, reference the Discussion in your PR description.
4. If skipping a Queue item, explain why in your PR description.
5. Ensure CI passes on Linux, Windows, and macOS before requesting review.
6. Request review from a Reviewer or Maintainer.
7. Once the PR is approved, document it in `wiki/` (see below) and push that to the same PR. A PR can't merge until it's documented.

### Documenting your change

The [OFP wiki](https://github.com/ofp-fs2open/fs2open.github.com/wiki) is published from the `wiki/` folder: each `.md` file there is one wiki page, and every merge that changes `wiki/` updates the wiki. Edit `wiki/`, not the wiki itself; edits made on the wiki are overwritten.

- **New engine or QtFRED feature:** add a row to `wiki/OFP-Engine-Features.md` or `wiki/OFP-QtFRED-Features.md`.
- **Fix to code OFP shares with SCP:** add a row to the top of `wiki/OFP-Bug-Fixes.md`. Fixes to OFP's own features don't need a row.
- **New or changed tables, SEXPs or Lua:** update the matching page (table pages are in `wiki/tables/`), and set its "OFP Revision" line to your PR. Mark new table fields with the OFP version badge, as the existing pages do. A new page also needs an entry in `wiki/_Sidebar.md` and `wiki/Home.md`.

Link your PR (`[#123](https://github.com/ofp-fs2open/fs2open.github.com/pull/123)`) wherever a row or revision line asks for one.

The **Documentation** check fails until the PR changes something under `wiki/`. If there is nothing to document (CI, build or internal changes), a maintainer adds the `no-docs` label instead.

---

## Code of Conduct

See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).
