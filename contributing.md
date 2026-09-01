---
title: Contributing
layout: default
nav_order: 99
---

# Contributing to the docs

## Editing an existing page
1. Open the page on the site and click **Edit this page on GitHub** at the bottom.
2. Make your changes in Markdown, then **Commit changes**.
3. The site rebuilds automatically in about a minute.

## Adding a new page
Create a new `.md` file in the right folder and give it front matter.

### Where files go
```
_<season>/<team>/<project>/<page>.md
```
For example, a new report page for this season's avionics team under the battery project:
`_2025-26/avionics/battery/report-1.md`

### Front matter template
Every page needs a block like this at the very top:

```yaml
---
title: Battery
layout: default
parent: Avionics      # the team/project this page belongs to
---
```

- **Team/Project landing pages** (the `index.md` in a team folder) use `has_children: true`
  and no `parent`.
- **Pages inside a team** use `parent: <Team Name>`.
- **Pages inside a project** use `parent: <Project Name>`.

## Starting a new season
1. Add a new collection in `_config.yml` under both `collections:` and
   `just_the_docs.collections:` (copy an existing year block, change the number).
2. Create the `_<new-season>/` folder with team subfolders.

## Naming rules
- Titles must be unique **within a season** (collections keep years separate).
- Use lowercase, hyphenated file names: `flight-computer.md`, not `Flight Computer.md`.

## Linking to other pages
Always build links with `site.baseurl` so they don't break on the `/docs` subpath.

**Link by file** (preferred — survives the file being moved or renamed):
```markdown
[See the parachutes page]({% raw %}{% link _2025-26/recovery/parachutes.md %}{% endraw %})
```

**Link by URL path** (the collection name in the URL drops the leading `_`):
```markdown
[See the parachutes page]({% raw %}{{ site.baseurl }}{% endraw %}/2025-26/recovery/parachutes/)
```

**Link to a heading on another page** — take the heading, lowercase it, replace
spaces with hyphens, and add it after `#`:
```markdown
[packing procedure]({% raw %}{{ site.baseurl }}{% endraw %}/2025-26/recovery/parachutes/#packing-procedure)
```

## Adding and linking PDFs
There is no upload button — a PDF is committed to the repo like any other file.

### 1. Add the file
Put shared PDFs in the central assets folder:
```
assets/pdfs/<name>.pdf
```
On GitHub: open the `assets/pdfs/` folder, then **Add file -> Upload files**, drag the
PDF in, and **Commit changes**. Use lowercase, hyphenated, dated names, e.g.
`2025-recovery-drop-test.pdf`.

### 2. Link to it (opens / downloads)
```markdown
[Recovery drop-test report (PDF)]({% raw %}{{ site.baseurl }}{% endraw %}/assets/pdfs/2025-recovery-drop-test.pdf)
```

### 3. Embed it inline (displays on the page)
Markdown pages accept raw HTML, so drop in an `<iframe>`:
```html
<iframe src="{% raw %}{{ site.baseurl }}{% endraw %}/assets/pdfs/2025-recovery-drop-test.pdf"
        width="100%" height="600px" style="border: 1px solid #ccc;">
  This browser can't display embedded PDFs.
  <a href="{% raw %}{{ site.baseurl }}{% endraw %}/assets/pdfs/2025-recovery-drop-test.pdf">Download it instead.</a>
</iframe>
```

See the **Drop Test** page under Recovery 2025-26 for a working example of all three.

### Embedding an image
Same idea, from `assets/images/`:
```markdown
![Airframe diagram]({% raw %}{{ site.baseurl }}{% endraw %}/assets/images/airframe-diagram.png)
```

> **Large files:** GitHub Free includes only 1 GB of Git LFS storage. For big CAD
> exports, video, or many large PDFs, use Git LFS or link out to Google Drive
> instead of committing them directly.