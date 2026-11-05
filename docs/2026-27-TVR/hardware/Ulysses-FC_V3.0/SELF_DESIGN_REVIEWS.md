# SELF DESIGN REVIEWS

## Date 04/11/2026

- Add some solder mask exp on impedence all RF traces to reduce insertion loss dues to solder mask, keep mask near TVS or any other component.

- Via jump CAN Diff pair for mezzanine carefully, add a few GND vias around transition to keep field contained and remove GND between all Diff pair to keep the coupling within the pair

- Edit rules to some add good sliver for any Z controlled traces to reduce field coupling with same layer pour. Think about return paths, no cutout or anything stupid on reference plan

- Highly advised to use some decent sliver for all, makes rework safer

- Recheck impedence profiles, verify Dk's and modify since CAN need to be via jumped now

## Date 05/11/2026

- +3V3 via for Pin 1 on SX1262 neads to be moves, hitting shield pads

- FEM IC Pin 35 decoupling cap can be placed by rotating Pin 1 cap 90 deg and move it a bit

- Move C107 a bit out for some clearance

- Do not forget teardrops on all CI traces and can use thermal relief on decoupling and bulk cap in radio section.(Semtech also agrees)

- Remove thermal relief on any +12V hole / pad (J14)

- Y did u remove thermal vias on SX1262 EP? Stupid, add it back

- Pin 24 on SX1262 not routed

- Length of mezzanine CAN Diff pair could a bit more matched (at around 0.6mm diff now), not super important. (Remember Rick Hartley lecture on Diff Pairs)
