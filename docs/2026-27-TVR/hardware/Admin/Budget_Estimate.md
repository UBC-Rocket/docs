# Thrust Vectoring (TVR) Hardware Budget Estimate

---

## Contents

1. Summary
2. Needs (required this year)
3. General items (required for the build)
4. Wants (value-add boards)
5. Component breakdown per board
6. What is not in this ask
7. The ask

---

## 1. Summary

This year's TVR avionics is a custom multi-board stack. This proposal splits the ask into **needs** (required to build and fly this year) and **wants** (boards that add real value but the vehicle can fly without). Two totals are given so the funding can be scaled: a **minimum (800 CAD)** and a **full build (1098 CAD)**.

Component costs are for **2 of each board** (spares for rework and respin). PCB fabrication is quoted for **5 boards** each (minimum order), sized from JLCPCB, excluding any JLC sponsorship or discounts, so the real cost may come in lower. The LiPo battery, motors, and propellers are already in hand and are not part of this ask.

| Tier | Amount |
|:-----|-------:|
| Needs (boards + GNSS antenna required this year) | 575 CAD |
| General items (wires, supplies, shipping) | 225 CAD |
| **Minimum (needs + general items)** | **800 CAD** |
| Wants (value-add boards) | 298 CAD |
| **Full build (everything)** | **1098 CAD** |

**If the full build is funded, every board and embedded system in TVR is built in house. All the custom electronics, from the flight controller to the motor drive to the charger to the telemetry ground station, are our own design. The only off-the-shelf hardware is the physical components that are not practical to build ourselves, like the motors, battery, and Dynamixel servos.**

---

## 2. Needs (required this year)

These are the core of the vehicle and its bring-up. Without them, TVR does not fly.

| Item | What it is for | Components (2 boards) | PCB (5 boards) | Total |
|:-----|:---------------|:---------------------:|:--------------:|------:|
| **FC PCB** (8-layer) | Flight controller: runs the real-time control loop that keeps the vehicle stable, fuses IMU/baro/GNSS/LiDAR data, drives the motors and gimbal. | 278 | 123 | 401 CAD |
| **Backplane PCB** (4-layer) | Power distribution to the stack, battery voltage/current monitoring, cell monitoring and passive balancing, servo and camera rails. | 47 | 10 | 57 CAD |
| **GNSS Antenna** (1x) | Feeds the RTK GNSS on the FC, for positioning and as a redundant height source. Required for the FC to function. | | | 80 CAD |
| **Test PCB** (2-layer) | Breakout for the board-to-board connectors, used during bring-up to probe and validate the stack before full assembly. Needed to safely bring up the FC and backplane. | 14 | 6 | 20 CAD |
| **LiDAR PCB** (2-layer) | Small carrier board for the LiDAR height sensor, mounted separately from the FC for placement. Provides precise low-altitude height for the control loop. | 13 | 4 | 17 CAD |
| | **Needs subtotal** | | | **575 CAD** |

**Note:** the FC is costed at 8-layer fabrication (123 CAD) as a worst case. It may be achievable in 6 layers, which would drop the fab to ~43 CAD and save ~80 CAD. This is confirmed during layout.

---

## 3. General items (required for the build)

Not a single board, but required to build, assemble, and operate the stack.

| Item | What it is for | Cost |
|:-----|:---------------|-----:|
| **Basic items** (wires, connectors, misc) | Interconnects, board-to-board and battery connectors, general assembly hardware. | 75 CAD |
| **Soldering supplies** | Solder, paste, flux, and consumables for fine-pitch reflow assembly. | 75 CAD |
| **Shipping** | Shipping for PCBs and component orders. | 75 CAD |
| | **General items subtotal** | **225 CAD** |

---

## 4. Wants (Adds real value, but the vehicle can fly without them)

Each adds capability; the charger in particular has value beyond TVR, COTS is intrested in them to put them inside rocket as well. Ranked roughly by value.

| Board | What it is for | Components (2 boards) | PCB (5 boards) | Total |
|:------|:---------------|:---------------------:|:--------------:|------:|
| **ESC PCB** (6-layer) | Custom dual-channel motor controller. Strong want and a major capability/learning piece, but a commercial ESC can fly the vehicle in the meantime. | 138 | 43 | 181 CAD |
| **USB-C PD Charger (DFT)** (4-layer) | 100W USB-C PD bench charger for the LiPo, to charge packs in the field without a separate charger. **Shared value: the COTS hardware wants it for their stack too, so one board serves both teams.** | 78 | 10 | 88 CAD |
| **Custom Telemetry GND Station** (4-layer) | Ground-side board for the 915 MHz telemetry link, a matched ground station for our custom radio. | 19 | 10 | 29 CAD |
| | **Wants subtotal** | | | **298 CAD** |

For now, these functions are covered by off-the-shelf options: a commercial ESC drives the motors, a bought charger charges the packs, and off-the-shelf ground hardware handles telemetry. The vehicle can fly and operate this way.

**Funding the wants replaces those off-the-shelf options with our own designs, making the entire TVR hardware system in house.** The custom ESC replaces the commercial ESC, our charger replaces the bought charger, and the telemetry ground station replaces the off-the-shelf ground hardware. With the full build, no part of the hardware is a commercial off-the-shelf unit, it is all our own design.

---

## 5. Component breakdown per board

Component quantities and costs are for **2 boards** except where noted. Small passives are consolidated into one line per board; explicit key parts (shunt, connectors, ICs, etc.) are listed separately. A USB-C connector is included on each board at 0.40 CAD. All parts from LCSC.

---

### 5.1 FC PCB

| Component | Qty | Cost (CAD) |
|:----------|:---:|-----------:|
| ZED-F9P RTK GNSS | 2 | 170.00 |
| IMU (@4.60) | 10 | 46.00 |
| Baro (@3.50) | 6 | 21.00 |
| Passives (decoupling, RF matching, protection, crystals, LEDs, test points) | 2 sets | 26.40 |
| SX1262 915 MHz radio | 2 | 6.40 |
| Board-to-board mezzanine (@1.25) | 4 | 5.00 |
| UWB interface connectors | 4 | 2.00 |
| USB-C connector | 2 | 0.80 |
| STM32H745 MCU | 2 | 0.00 (owned) |
| **Component total (2 boards)** | | **277.60** |
| PCB fabrication (5 boards, 8-layer) | | 123.00 |
| **FC board total** | | **401 CAD** |

**Note:** IMU and baro are a shared purchase (10 IMU, 6 baro total): each TVR FC board carries 3 IMU and 1 baro, so 6 IMU and 2 baro across the 2 TVR boards, plus 1 IMU and 1 baro as TVR spares; the remaining 3 IMU and 3 baro cover the COTS board. MCU reused from the previous FC, so not purchased. The LiDAR now sits on its own board (5.7).

---

### 5.2 ESC PCB

| Component | Qty | Cost (CAD) |
|:----------|:---:|-----------:|
| FET IAUC120N04S6L005 | 24 | 55.20 |
| Passives (polymer/tantalum bulk, ceramics, gate resistors, bootstrap, LEDs) | 2 sets | 34.00 |
| Gate driver DRV8353S | 4 | 17.60 |
| MCU STM32G070CBT6 | 4 | 7.60 |
| Fuel gauge BQ34Z100 | 2 | 5.20 |
| Motor bullet connectors | 12 | 5.00 |
| Board-to-board mezzanine (@1.25) | 4 | 5.00 |
| ST-Link/debug header | 2 | 4.00 |
| Battery + balance connectors | 2 sets | 2.60 |
| Current-sense shunt (Kelvin) | 2 | 0.98 |
| USB-C connector | 2 | 0.80 |
| **Component total (2 boards)** | | **137.98** |
| PCB fabrication (5 boards, 6-layer) | | 43.00 |
| **ESC board total** | | **181 CAD** |

---

### 5.3 Backplane PCB

| Component | Qty | Cost (CAD) |
|:----------|:---:|-----------:|
| Passives (bulk, decoupling, protection TVS, balance bleed resistors, LEDs, test points, camera power) | 2 sets | 13.80 |
| Rail regulation (5V buck, 12V Dynamixel buck, 3V3 LDO) | 2 sets | 9.00 |
| Power/load-switch FETs (@0.70) | 12 | 8.40 |
| Cell monitor + passive balance (BQ76942) | 2 | 5.34 |
| Board-to-board mezzanine (@1.25) | 4 | 5.00 |
| Other connectors (balance, power, camera, servo out) | 2 sets | 5.00 |
| USB-C connector | 2 | 0.80 |
| **Component total (2 boards)** | | **47.34** |
| PCB fabrication (5 boards, 4-layer) | | 10.00 |
| **Backplane board total** | | **57 CAD** |

---

### 5.4 USB-C PD Charger DFT PCB

| Component | Qty | Cost (CAD) |
|:----------|:---:|-----------:|
| Passives (bulk poly/tantalum/ceramic, PD protection TVS/ESD, test points, LEDs) | 2 sets | 24.90 |
| Charger FETs (higher-voltage, for up to 10-14S range) | 2 sets | 12.00 |
| BQ25756 bidirectional buck-boost charger | 2 | 10.06 |
| OLED display | 2 | 8.00 |
| TPS25751 PD controller | 2 | 5.54 |
| BQ76942 monitor | 2 | 5.34 |
| Small MCU (OLED/UI + digital charge V/I control) | 2 | 4.00 |
| Battery connectors (DC jack, XT60, JST, terminal blocks) | 2 sets | 3.08 |
| Charger inductor(s) | 2 sets | 2.42 |
| Charge voltage/current adjust (buttons) | 2 sets | 2.00 |
| USB-C connector | 2 | 0.80 |
| **Component total (2 boards)** | | **78.14** |
| PCB fabrication (5 boards, 4-layer) | | 10.00 |
| **Charger board total** | | **88 CAD** |

---

### 5.5 Custom Telemetry GND Station PCB

| Component | Qty | Cost (CAD) |
|:----------|:---:|-----------:|
| Passives (RF matching, 3V3 LDO, decoupling, misc) | 2 sets | 6.60 |
| SX1262 915 MHz radio | 2 | 6.40 |
| FTDI USB-UART bridge | 2 | 5.00 |
| USB-C connector | 2 | 0.80 |
| SMA connector | 2 | 0.60 |
| **Component total (2 boards)** | | **19.40** |
| PCB fabrication (5 boards, 4-layer) | | 10.00 |
| **Telemetry GND board total** | | **29 CAD** |

---

### 5.6 Test PCB

| Component | Qty | Cost (CAD) |
|:----------|:---:|-----------:|
| Passives (3V3 LDO, breakout headers, decoupling) | 2 sets | 5.20 |
| Board-to-board mezzanine (@1.25) | 4 | 5.00 |
| 5V buck | 2 | 2.96 |
| USB-C connector | 2 | 0.80 |
| **Component total (2 boards)** | | **13.96** |
| PCB fabrication (5 boards, 2-layer) | | 6.00 |
| **Test board total** | | **20 CAD** |

---

### 5.7 LiDAR PCB

| Component | Qty | Cost (CAD) |
|:----------|:---:|-----------:|
| LiDAR sensor (@5.70) | 2 | 11.40 |
| Connector to FC + decoupling passives | 2 sets | 1.50 |
| **Component total (2 boards)** | | **12.90** |
| PCB fabrication (5 boards, 2-layer) | | 4.00 |
| **LiDAR board total** | | **17 CAD** |

---

### 5.8 Board cost totals

| Board | Total |
|:------|------:|
| FC PCB | 401 CAD |
| ESC PCB | 181 CAD |
| USB-C PD Charger DFT | 88 CAD |
| Backplane PCB | 57 CAD |
| Custom Telemetry GND Station | 29 CAD |
| Test PCB | 20 CAD |
| LiDAR PCB | 17 CAD |
| **All boards total** | **793 CAD** |

---

## 6. What is not in this ask

- **LiPo battery, motors, propellers** already in hand.
- **STM32H745 MCU** reused from the previous flight controller.

---

## 7. The ask

- **Minimum: 800 CAD** (FC + Backplane + Test PCB + LiDAR PCB + GNSS antenna + general items).
- **Full build: 1098 CAD** (adds the ESC, the shared-use USB-C PD charger, and the telemetry ground station).

A 25% margin of safety is added on the full build to cover price changes, stock substitutions, order-quantity minimums, and rework or respins.

| Item | Amount |
|:-----|-------:|
| Full build | 1098 CAD |
| Margin of safety (25%) | 274 CAD |
| **Total budget requested** | **1372 CAD** |

**Funding the full build means the entire TVR hardware system is built in house, with no commercial off-the-shelf units anywhere.** Every board and every hardware subsystem, from the motor drive to the charger to the ground station, is our own design. The charger is the strongest single want to fund even on a tight budget, since it is shared with the COTS Rocket team and pays back across two projects.

---

*Component prices are LCSC estimates at time of writing; final cost depends on stock and order quantity. PCB fabrication quoted for 5 boards from JLCPCB, excluding sponsorship/discounts.*