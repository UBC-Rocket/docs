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
2025-26 → Avionics → Battery Project automatically.

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

!!! tip
    A common alternative to `.pages` files is numeric filename prefixes
    (`01-intro.md`, `02-setup.md`). Pick one convention and stick with it.

## Starting a new season

Just create a new folder under `docs/`, e.g. `docs/2026-27/`, and add an
`index.md`. To put it at the top of the sidebar, add it to the top-level
`docs/.pages` file above the older seasons. No config changes needed.

## Nesting depth

Unlike the previous setup, there is **no three-level limit** — nest folders as
deep as the material genuinely needs. Keep it reasonable for readability, but
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

There's no upload button — commit the PDF like any other file.

### 1. Add the file

Put shared PDFs in `docs/assets/pdfs/`. On GitHub: open that folder, then
**Add file → Upload files**, drag the PDF in, and **Commit changes**. Use
lowercase, hyphenated, dated names: `2025-recovery-drop-test.pdf`.

### 2. Link to it

```markdown
[Drop-test report (PDF)](../../../assets/pdfs/2025-recovery-drop-test.pdf)
```

Count the `../` back up to `docs/` from wherever your page lives.

### 3. Embed it inline

```html
<iframe src="../../../assets/pdfs/2025-recovery-drop-test.pdf"
        width="100%" height="600px" style="border: 1px solid #ccc;">
  <a href="../../../assets/pdfs/2025-recovery-drop-test.pdf">Download it instead.</a>
</iframe>
```

See **2025-26 → Recovery → Parachute → Drop Test** for a working example.

### Images

```markdown
![Airframe diagram](../../../assets/images/airframe-diagram.png)
```

!!! warning "Large files"
    GitHub Free includes only 1 GB of Git LFS storage. For big CAD exports,
    video, or many large PDFs, use Git LFS or link out to Google Drive instead
    of committing them directly.
