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
_<season>/<team>/<page>.md
```
For example, a new battery page for this season's avionics team:
`_2025-26/avionics/battery.md`

### Front matter template
Every page needs a block like this at the very top:

```yaml
---
title: Battery
layout: default
parent: Avionics      # the team this page belongs to
nav_order: 3          # position in the sidebar (lower = higher up)
---
```

- **Team landing pages** (the `index.md` in a team folder) use `has_children: true`
  and no `parent`.
- **Pages inside a team** use `parent: <Team Name>`.

## Starting a new season
1. Add a new collection in `_config.yml` under both `collections:` and
   `just_the_docs.collections:` (copy an existing year block, change the number).
2. Create the `_<new-season>/` folder with team subfolders.
3. Give the new season the lowest `nav_order` so it sits at the top.

## Naming rules
- Keep `nav_order` values spaced out (10, 20, 30) so you can insert pages later.
- Titles must be unique **within a season** (collections keep years separate).
- Use lowercase, hyphenated file names: `flight-computer.md`, not `Flight Computer.md`.
