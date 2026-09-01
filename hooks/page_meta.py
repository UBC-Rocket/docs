"""
Inserts last-updated date, created date, and author(s) immediately after the
page's <h1> title, instead of Material's default page-footer location.

Dates come from the git-revision-date-localized plugin (page.meta keys).
Authors are read directly from git log here, because the git-authors plugin
exposes authors only as Jinja template variables a Python hook cannot see.

When a commit's email is a GitHub noreply address
(e.g. "12345+octocat@users.noreply.github.com" or
"octocat@users.noreply.github.com") we recover the GitHub username from it and
render the author's avatar + a link to their profile. Contributors who commit
with a different email are shown as a plain name.
"""

import html
import re
import subprocess
from functools import lru_cache

# Matches GitHub noreply emails, capturing the username after an optional
# "<numeric-id>+" prefix.
_NOREPLY_RE = re.compile(
    r"^(?:\d+\+)?([A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?)@users\.noreply\.github\.com$"
)


def _github_login(email):
    """Return the GitHub username for a noreply email, else None."""
    match = _NOREPLY_RE.match(email.strip())
    return match.group(1) if match else None


@lru_cache(maxsize=None)
def _authors_for(path):
    """Return an ordered list of (name, github_login_or_None) for this file.

    Ordered most-recent-committer-first, de-duplicated by name.
    """
    try:
        out = subprocess.run(
            ["git", "log", "--follow", "--format=%an%x00%ae", "--", path],
            capture_output=True, text=True, check=True,
        ).stdout
    except Exception:
        return ()
    seen = []
    names = set()
    for line in out.splitlines():
        name, _, email = line.partition("\x00")
        name = name.strip()
        if name and name not in names:
            names.add(name)
            seen.append((name, _github_login(email)))
    return tuple(seen)


def _render_author(name, login):
    """Render one author: an avatar + profile link if we know their GitHub
    username, otherwise just their escaped name."""
    safe_name = html.escape(name)
    if not login:
        return f'<span class="page-meta__author">{safe_name}</span>'
    avatar = f"https://github.com/{login}.png?size=40"
    return (
        f'<a class="page-meta__author" href="https://github.com/{login}"'
        f' rel="noopener">'
        f'<img class="page-meta__avatar" src="{avatar}" alt="" loading="lazy"'
        f' width="16" height="16">{safe_name}</a>'
    )


def on_page_content(page_html, page, config, files):
    meta = getattr(page, "meta", {}) or {}

    updated = meta.get("git_revision_date_localized")
    created = meta.get("git_creation_date_localized")

    src_path = None
    try:
        src_path = page.file.abs_src_path
    except Exception:
        pass
    authors = _authors_for(src_path) if src_path else ()

    if not (updated or created or authors):
        return page_html

    bits = []
    if authors:
        rendered = [_render_author(name, login) for name, login in authors[:3]]
        shown = ", ".join(rendered)
        if len(authors) > 3:
            shown += f" +{len(authors) - 3}"
        bits.append(f'<span class="page-meta__authors">By {shown}</span>')
    if updated:
        bits.append(f'<span class="page-meta__updated">Last updated: {updated}</span>')
    if created:
        bits.append(f'<span class="page-meta__created">Created: {created}</span>')

    meta_html = (
        '<div class="page-meta">'
        + ' <span class="page-meta__sep">&middot;</span> '.join(bits)
        + '</div>'
    )

    match = re.search(r"</h1>", page_html)
    if match:
        idx = match.end()
        return page_html[:idx] + meta_html + page_html[idx:]
    return meta_html + page_html
