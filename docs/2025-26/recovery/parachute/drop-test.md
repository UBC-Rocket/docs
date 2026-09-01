# Drop Test

This page shows how to link to another page, link to a PDF, and embed a PDF
inline. In MkDocs you use plain relative paths — no `baseurl` needed.

## Related pages

For canopy sizing and packing, see [parachutes](parachutes.md).

Link straight to a heading on another page:
[packing procedure](parachutes.md#packing-procedure).

To link *up* to another team, use relative paths with `../`:
[Avionics firmware](../../avionics/firmware.md).

## Download the report

The full drop-test report as a PDF:

[Recovery drop-test report (PDF)](2025-recovery-drop-test.pdf)

## Read it inline

The same PDF embedded directly in the page:

<iframe src="../2025-recovery-drop-test.pdf"
        width="100%" height="600px" style="border: 1px solid #ccc;">
  This browser can't display embedded PDFs.
  <a href="../2025-recovery-drop-test.pdf">Download it instead.</a>
</iframe>

!!! note
    MkDocs has a callout syntax like this — handy for warnings, tips, and notes.
    It's one of the nice extras you get over Just the Docs.
