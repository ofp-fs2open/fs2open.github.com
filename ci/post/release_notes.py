# Builds the release notes for a release tag: OFP's own changes since the previous release,
# plus one line summarizing the upstream FSO merges folded in over the same window.
#
# Used from two places so both carry the same text:
#   - build-release.yaml passes the output as the GitHub release body
#   - main.py sends it to Nebula as the release notes
#
# Standard library only: build-release.yaml's create_release job does not install
# ci/post/requirements.txt. Kept runnable under Python 2.7 as well, so the notes for a tag
# can be checked by hand before a release goes out.

import argparse
import os
import re
import subprocess
import sys

SYNC_LOG_URL = "https://github.com/ofp-fs2open/fs2open.github.com/discussions/2"
SCP_COMMIT_URL = "https://github.com/scp-fs2open/fs2open.github.com/commit/"

# Above this many entries the anchor tag is almost certainly wrong rather than the window being
# genuinely huge. The OFP repository holds only OFP's own release tags, so CI cannot pick a
# pre-fork SCP tag, but a clone with SCP as a remote has a hundred of them and will reach back
# past the fork when no OFP tag lies below the one being described.
MAX_ENTRIES = 200

# A first-parent merge whose subject reads like one of OFP's upstream syncs is rolled into the
# upstream line. Any other merge is listed as a change of its own, so a pull request that was
# merged rather than squashed still appears.
UPSTREAM_SUBJECT = re.compile(r"\b(scp|upstream)\b", re.IGNORECASE)


def git(*args, **kwargs):
    """Run a git command, returning its stripped output, or an empty string if it failed.

    A failure is reported on stderr so that a broken checkout does not read the same as a
    legitimately empty result. Pass quiet=True where a non-zero exit is an expected answer.
    """
    quiet = kwargs.pop("quiet", False)
    try:
        return subprocess.check_output(("git",) + args, universal_newlines=True).strip()
    except (subprocess.CalledProcessError, OSError) as err:
        if not quiet:
            sys.stderr.write("release_notes: git {} failed: {}\n".format(" ".join(args), err))
        return ""


def previous_release_tag(tag):
    """The most recent release tag on the first-parent line below `tag`, or "" if there is none.

    --first-parent matters twice over: it keeps the search on OFP's own line of development, and
    it stops the 100-odd SCP release tags that arrive inside sync merges from being picked up as
    OFP's previous release.

    Weekly tags sit directly on master, so this resolves to the previous weekly release. A tag
    from cut-release.yaml sits on a side commit that is on no branch, so it is unreachable from
    any later tag: a cut release's window therefore reaches back to the last release in its own
    history, not to a preceding release candidate.
    """
    return git("describe", "--first-parent", "--tags", "--abbrev=0", "--match", "release_*",
               tag + "^", quiet=True)


def ofp_changes(prev, tag):
    """Subjects of OFP's own commits in the window.

    --first-parent --no-merges is what separates OFP's work from the upstream commits that
    arrive as a merge's second parent: every OFP pull request is squash merged onto the
    first-parent line, and squash merge already appends its "(#NN)" reference.
    """
    log = git("log", "--first-parent", "--no-merges", "--pretty=%s", "{}..{}".format(prev, tag))
    return [line for line in log.splitlines() if line.strip()]


def upstream_summary(prev, tag):
    """(newest SCP commit merged, total upstream commits, subjects of any other merges)."""
    merges = git("log", "--merges", "--first-parent", "--pretty=%H %s", "{}..{}".format(prev, tag))
    through = ""
    total = 0
    others = []
    for line in merges.splitlines():
        sha, _, subject = line.partition(" ")
        if not UPSTREAM_SUBJECT.search(subject):
            others.append(subject)
            continue
        side = git("rev-parse", "--short", sha + "^2")
        if not side:
            # A merge with no second parent, so there is nothing upstream to report for it
            continue
        if not through:
            # git log lists newest first, so the first upstream merge seen is the newest
            through = side
        count = git("rev-list", "--count", "{}^1..{}^2".format(sha, sha))
        total += int(count) if count.isdigit() else 0
    return through, total, others


def build_release_notes(tag, repo=""):
    """Render the notes for `tag` as Markdown. `repo` ("owner/name") adds a compare link."""
    prev = previous_release_tag(tag)
    if not prev:
        return ("Release {}.\n\nNo previous release tag was found below this one, "
                "so there is no change list for this build.\n".format(tag))

    entries = ofp_changes(prev, tag)
    through, upstream_count, other_merges = upstream_summary(prev, tag)
    entries += other_merges

    if len(entries) > MAX_ENTRIES:
        # Report the miss rather than dumping a pre-fork window into the release page
        return "\n".join([
            "Changes since `{}`.".format(prev),
            "",
            "{} commits is too many to list, which usually means the previous release tag was "
            "not found on this tag's own line of development.".format(len(entries)),
            "",
            "[Full commit list](https://github.com/{}/compare/{}...{})".format(
                repo or "ofp-fs2open/fs2open.github.com", prev, tag),
            "",
        ])

    lines = ["Changes since `{}`.".format(prev), ""]

    if entries:
        lines += ["## OFP changes", ""]
        lines += ["- " + entry for entry in entries]
        lines.append("")

    if upstream_count:
        lines += ["## Upstream FSO", ""]
        # The count is every commit the merge brought in, which is what the Sync Log entry for
        # the same sync reports. Some of them are SCP's own merge commits.
        lines.append("Merged FSO upstream through [scp-fs2open@{}]({}{}) ({} commit{}). "
                     "The [Upstream Sync Log]({}) records what each sync brought in."
                     .format(through, SCP_COMMIT_URL, through, upstream_count,
                             "" if upstream_count == 1 else "s", SYNC_LOG_URL))
        lines.append("")

    if not entries and not upstream_count:
        lines += ["No changes recorded since `{}`.".format(prev), ""]

    if repo:
        lines.append("[Full commit list](https://github.com/{}/compare/{}...{})".format(repo, prev, tag))

    return "\n".join(lines).rstrip() + "\n"


def main():
    parser = argparse.ArgumentParser(description="Build the release notes for a release tag")
    parser.add_argument("tag", help="Release tag, e.g. release_26_1_0-20261005")
    parser.add_argument("--repo", default=os.environ.get("GITHUB_REPO", ""),
                        help="owner/name, used for the compare link (default: $GITHUB_REPO)")
    args = parser.parse_args()
    sys.stdout.write(build_release_notes(args.tag, args.repo))


if __name__ == "__main__":
    main()
