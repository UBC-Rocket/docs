# Contributing to the docs

## Editing an existing page

1. Open the page on the site and click the **pencil icon** at the top right.
2. Edit the Markdown on GitHub, then **Commit changes**.
3. The site rebuilds automatically in about a minute.

## Adding a new page

Just add a Markdown file to the right folder in `docs/`. The sidebar updates
automatically to match the folder structure — **no front matter required.**

### Where files go

```
docs/<season>/<team>/<project>/<page>.md
```

For example, a new report for this season's avionics battery project:
`docs/2025-26/avionics/battery/report-2.md`

That's it. The page appears in the sidebar under
2025-26 → Avionics → Battery automatically.

### Folder landing pages

An `index.md` inside a folder becomes that folder's landing page (the page you
see when you click the section name). Every team/project folder should have one.

## Controlling order

By default, pages sort **alphabetically**. To set an explicit order, add a
`.pages` file to the folder listing the items in the order you want:

```yaml
nav:
  - index.md
  - firmware.md
  - hardware.md
  - battery
```

Folders and files not listed fall to the bottom alphabetically. You only need a
`.pages` file in folders where order matters — elsewhere alphabetical is fine.

### Fixing mangled section names

MkDocs auto-generates sidebar titles from folder names and turns hyphens into
spaces (so `2025-26` shows as "2025 26"). To force the exact text, add a
`title:` line to that folder's `.pages` file:

```yaml
title: 2025-26
nav:
  - index.md
  - avionics
  - recovery
```

!!! tip
    A common alternative to `.pages` files is numeric filename prefixes
    (`01-intro.md`, `02-setup.md`). Pick one convention and stick with it.

## Starting a new season

Just create a new folder under `docs/`, e.g. `docs/2026-27/`, and add an
`index.md`. To put it at the top of the sidebar, add it to the top-level
`docs/.pages` file above the older seasons. No config changes needed.

## Nesting depth

There is **no three-level limit** — nest folders as deep as the material
genuinely needs. Keep it reasonable for readability, but
season → team → project → sub-project → page all works.

## Linking to other pages

Use **relative paths to the `.md` file** (MkDocs rewrites them and warns you at
build time if a link breaks):

```markdown
[See parachutes](parachutes.md)
[A heading](parachutes.md#packing-procedure)
[Up to another team](../../avionics/firmware.md)
```

## Adding and linking PDFs

Keep a PDF **in the same project folder as the page that uses it.** MkDocs copies
any non-Markdown file in `docs/` into the built site at the same location, so a
co-located PDF just works — and your links stay short with no `../` climbing.

### 1. Add the file

Put the PDF directly in the project folder alongside its pages, e.g.:

```
docs/2025-26/recovery/parachute/
├── index.md
├── parachutes.md
├── drop-test.md
└── 2025-recovery-drop-test.pdf     ← here
```

On GitHub: open the project folder, then **Add file → Upload files**, drag the
PDF in, and **Commit changes**. Use lowercase, hyphenated, dated names:
`2025-recovery-drop-test.pdf`. The `.pages` nav ignores raw PDFs, so it won't
appear in the sidebar.

### 2. Link to it

Because the PDF is in the same folder as the page, just use its filename:

```markdown
[Drop-test report (PDF)](2025-recovery-drop-test.pdf)
```

If you reference a PDF in a *different* folder, use a relative path with `../`
to reach it (e.g. a page one level up: `parachute/2025-recovery-drop-test.pdf`).

### 3. Embed it inline

```html
<iframe src="../2025-recovery-drop-test.pdf"
        width="100%" height="600px" style="border: 1px solid #ccc;">
  <a href="../2025-recovery-drop-test.pdf">Download it instead.</a>
</iframe>
```

See **2025-26 → Recovery → Parachute → Drop Test** for a working example.

### Images

Same idea — keep an image next to the page that uses it and reference it by name:

```markdown
![Airframe diagram](airframe-diagram.png)
```

!!! note "Filenames must match exactly"
    The site builds in `--strict` mode, so a link to a PDF that isn't there
    (typo, wrong case, or moved file) turns the build **red** instead of
    shipping a dead link. Match the committed filename exactly, case included.