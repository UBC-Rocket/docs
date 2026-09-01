"""
Inserts last-updated date, created date, and author(s) immediately after the
page's <h1> title, instead of Material's default page-footer location.

Dates come from the git-revision-date-localized plugin (page.meta keys).
Authors are read directly from git log here, because the git-authors plugin
exposes authors only as Jinja template variables a Python hook cannot see.
"""

import re
import subprocess
from functools import lru_cache


@lru_cache(maxsize=None)
def _authors_for(path):
    """Return an ordered list of author names who touched this file."""
    try:
        out = subprocess.run(
            ["git", "log", "--follow", "--format=%an", "--", path],
            capture_output=True, text=True, check=True,
        ).stdout
    except Exception:
        return ()
    seen = []
    for name in out.splitlines():
        name = name.strip()
        if name and name not in seen:
            seen.append(name)
    return tuple(seen)


def on_page_content(html, page, config, files):
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
        return html

    bits = []
    if authors:
        shown = ", ".join(authors[:3])
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

    match = re.search(r"</h1>", html)
    if match:
        idx = match.end()
        return html[:idx] + meta_html + html[idx:]
    return meta_html + html
