# POST-FAB DEFECTS

Running log of defects found after fabrication. Capture immediately on discovery,
before fixing or moving on. Every entry gets a mitigation for THIS board and an
action for the next rev, so "not fixed here" is always a decision, not an oversight.

Entries are grouped by severity, most severe first. Within a severity, newest at top.

**Board:** Ulysses Flight Controller V3.0   
**Fab order date:** 13/11/2026   
**Log started:** 24/11/2026     
**Maintainer:** Gugan Sivakumar    

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
- **Mitigation-Planned:** If and when need arises, will be mitigated.
- **Deferred:** intentionally not touched on this board; fix in next rev only.
- **Fixed-next-rev:** correction captured in the next rev source files.

**Priority**
- **High:** Will likely be used immediately or near future. Need to be functional after bring up.
- **Medium:** May not be used in near future but will be used eventually. Rework some time after bring up, when needed is accepetable
- **Low:** Will likely not be used in near future but may be used in eventually. Rework if and when subsystem is needed. 
---

# CRITICAL

_None found._

---

# MAJOR

### M001: CAN transceiver TXD/RXD swapped
- **Date found:** 24/11/2026
- **Status:** Mitigation-Planned / Deferred
- **Subsystem:** CAN-FD (TCAN1044)
- **Subsystem-Priority:** Low
- **Description:** MCU-to-transceiver TXD/RXD routed as a UART-style crossover (TX to RX) instead of straight-through. A controller-to-transceiver link is straight (MCU TX to transceiver TXD, MCU RX to transceiver RXD); the transceiver is a translator, not a peer.
- **Root cause:** Applied the UART crossover mental model to a transceiver interface.
- **Impact on this board:** CAN non-functional. Likely not going to used , so no operational impact.
- **Mitigation (this board):** None needed; CAN unused.
- **Fix (next rev):** Route TXD/RXD straight-through.
- **Process lesson:** Do not assume conventions. Always double check when designing something you have not done before.
- **Technical lesson:** A Transciever is a PHY not a device / controller. It generates the voltage levels in a bus. In a PHY, TXD means the data to be sent to the bus and RXD mean the data recieved from the bus so TXD to controller TX and RXD to Controller RX is correct.
- **Verification:**

---

# MINOR

### m001: RF-line ESD diode VRWM under-spec'd
- **Date found:** 24/11/2026
- **Status:** Mitigated (DNP)
- **Subsystem:** RTK-GNSS, RADIO
- **Subsystem-Priority:** RTK-GNSS-High, RADIO-Low
- **Description:** ESD/TVS part on the RF line spec'd at 3.3V VRWM, but the 1W (+30 dBm) carrier is about 7 Vrms, about 10 Vpeak into 50 ohm under ideal conditions (higher under realistic VSWR). The part would clamp into or be damaged by the transmit signal.
- **Root cause:** Spec'd the protection voltage as if for a digital or logic signal; did not work out the RF carrier Vpeak/Vrms before selecting the part.
- **Impact on this board:** If populated, distorts or absorbs TX and/or destroys the diode.
- **Mitigation (this board):** DNP the part or order new TVS with higher VRWM and same footprint (0201). If DNP, lose RF-line ESD protection (acceptable).
- **Fix (next rev):** Low-capacitance TVS with VRWM comfortably above the worst-case (post-VSWR) RF peak (well above 10 Vpeak, for example 15V VRWM or higher), keeping the low capacitance that made the original part suitable.
- **Process lesson:** Flagged this while designing, was meaning to look into RF Carrier wave diagrams, Vpeak and Vrms since there is nothing called logic level in RF signal. Slipped my mind and specced it like for a digital signal.
- **Technical lesson:**RF carrier: Vrms = sqrt(P * Z), Vpeak = Vrms * sqrt(2), derate for VSWR. (Just ohms law)
- **Verification:**

---

# COSMETIC

_None found._

---

## Next Rev action list (rolled up from above, most severe first)

- M001, CAN TXD/RXD straight-through
- m001, RF-line TVS: higher VRWM, low-cap part

---

## Process lessons (carry forward, not board-specific)
Patterns behind the defects, so the same mistakes does not occur again.

- Question everything and Never use something you do not understand

- Properly review even the simplest and straight forward section