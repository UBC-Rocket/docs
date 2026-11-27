# POST-FAB DEFECTS

Running log of defects found after fabrication. Capture immediately on discovery,
before fixing or moving on. Every entry gets a mitigation for THIS board and an
action for the next rev, so "not fixed here" is always a decision, not an oversight.

Entries are grouped by severity, most severe first. Within a severity, newest at top.

**Board:** <board name and rev>
**Fab order date:** <dd/mm/yyyy>
**Log started:** <dd/mm/yyyy>
**Maintainer:** <name>

---

## Tags

**Severity**
- **Critical:** board will not function, is unsafe, or blocks bring-up. Address before use.
- **Major:** a subsystem is impaired or needs rework to work correctly.
- **Minor:** works with a mitigation (DNP, bodge, firmware workaround); fix cleanly next rev.
- **Cosmetic:** silk, label, or aesthetic; no functional impact.

**Status**
- **Open:** found, not yet mitigated.
- **Mitigated:** handled on this board (DNP, bodge, or workaround in place).
- **Mitigation-Planned:** if and when need arises, will be mitigated.
- **Deferred:** intentionally not touched on this board; fix in next rev only.
- **Fixed-next-rev:** correction captured in the next rev source files.

**Priority**
- **High:** will likely be used immediately or near future. Needs to be functional after bring-up.
- **Medium:** may not be used in near future but will be used eventually. Rework some time after bring-up, when needed, is acceptable.
- **Low:** will likely not be used in near future but may be used eventually. Rework if and when the subsystem is needed.

---

# CRITICAL

Use template below for each entry
---

# MAJOR

Use template below for each entry
---

# MINOR

Use template below for each entry
---

# COSMETIC

Use template below for each entry
---

## New defect, template (copy into the matching severity section)

```
### <ID>: <short title>
- **Date found:**
- **Status:**
- **Subsystem:**
- **Subsystem-Priority:**
- **Description:**
- **Root cause:**
- **Impact on this board:**
- **Mitigation (this board):**
- **Fix (next rev):**
- **Process lesson:**
- **Technical lesson:**
- **Mitigation Verification Plan:**
- **Mitigation Result**:
```

ID scheme: prefix by severity so the ID carries its own severity (C### critical,
M### major, m### minor, X### cosmetic). Number within each severity in order found.

---

## Next Rev action list (rolled up from above, most severe first)

- <ID>, <short action>

---

## Process lessons (carry forward, not board-specific)
Patterns behind the defects, so the same mistakes do not occur again.

- <lesson>