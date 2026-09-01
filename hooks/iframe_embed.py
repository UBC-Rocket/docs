"""
Lets pages embed an iframe using ordinary markdown image syntax, keyed off the
alt text:

    ![embed](https://viewer.diagrams.net/?...)

Any image whose alt text is "embed" or "iframe" is replaced with a responsive
<iframe> pointing at the same URL. Every other image is left untouched, so
normal pictures (![hardware stack](stack.png)) still render as <img>.

This runs on the rendered HTML (on_page_content) so it doesn't have to re-parse
markdown; python-markdown has already turned ![alt](url) into <img alt src>.
"""

import re

_IMG_RE = re.compile(r"<img\b[^>]*>", re.IGNORECASE)
_TRIGGERS = {"embed", "iframe"}


def _attr(tag, name):
    """Return the value of attribute `name` in an HTML tag, or None."""
    match = re.search(rf'{name}\s*=\s*"([^"]*)"', tag, re.IGNORECASE)
    if match:
        return match.group(1)
    match = re.search(rf"{name}\s*=\s*'([^']*)'", tag, re.IGNORECASE)
    return match.group(1) if match else None


def _iframe(url):
    # `url` keeps whatever entity-encoding python-markdown produced (e.g. &amp;),
    # which is already correct inside an HTML attribute.
    return (
        f'<iframe src="{url}" '
        f'width="100%" height="600px" loading="lazy" '
        f'style="border: 1px solid #ccc;">'
        f'<a href="{url}">Open it in a new tab instead.</a>'
        f'</iframe>'
    )


def _replace(match):
    tag = match.group(0)
    alt = (_attr(tag, "alt") or "").strip().lower()
    if alt in _TRIGGERS:
        src = _attr(tag, "src")
        if src:
            return _iframe(src)
    return tag


def on_page_content(html, page, config, files):
    if "<img" not in html:
        return html
    return _IMG_RE.sub(_replace, html)
