# UBC Rocket Docs

Central documentation site for UBC Rocket, built with
[Just the Docs](https://just-the-docs.com/) and hosted on GitHub Pages.

**Live site:** https://ubc-rocket.github.io/docs/

## Structure
Docs are organized by season → team → page. Each season is a Jekyll
*collection* (a folder starting with `_`, e.g. `_2025-26/`), which lets team
names repeat across years without clashing.

```
_2025-26/
  avionics/
    index.md        # team landing page
    firmware.md     # a page under Avionics
  recovery/
    ...
```

## Editing
Click **Edit this page on GitHub** at the bottom of any page on the live site,
or edit the Markdown files directly in this repo. See
[contributing.md](contributing.md) for the full guide.

## Running locally (optional)
```bash
bundle install
bundle exec jekyll serve
# open http://localhost:4000/docs/
```
