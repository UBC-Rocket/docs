# UBC Rocket Docs

Central documentation site for UBC Rocket, built with
[MkDocs Material](https://squidfunk.github.io/mkdocs-material/) and hosted on
GitHub Pages.

**Live site:** https://ubc-rocket.github.io/docs/

## How it works

The sidebar mirrors the folder structure under `docs/` automatically (via the
awesome-pages plugin) — add a Markdown file to a folder and it shows up in the
nav. No front matter needed.

```
docs/
  2025-26/
    avionics/
      index.md          # team landing page
      firmware.md
      battery/          # nest as deep as you like
        index.md
        report-1.md
    recovery/
      parachute/
        index.md
        parachutes.md
        drop-test.md
  assets/pdfs/          # PDFs live here
```

Order within a folder is alphabetical unless a `.pages` file specifies it.

## Editing

Click the pencil icon on any page, or edit files in this repo. See
[docs/contributing.md](docs/contributing.md).

## Running locally

```bash
pip install -r requirements.txt
mkdocs serve
# open http://127.0.0.1:8000/
```
