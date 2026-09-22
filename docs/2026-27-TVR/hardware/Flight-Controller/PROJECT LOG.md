# Flight-Controller Project Log

## Date: 13/09/2026

- We are going for a new FC for TVR cause PCIe sucks, Very diffuclt to mount and we are not using half the things in present FC.

- I am just going to desolder the MCU(STM32H745) from present FC and solder it on new FC. Also will share a lot of components with COTS to reduce costs.

- Sensor Board is merged with this new FC so all the sensors will be on FC itself. Stack now is FC, backplane and ESC.

- Added MCU Footprint to Altium library.

- Finished MCU PWR and Debug schematics.

- Shortlisted a few Board to Board connector, will make a decision soon.

- Added a pin assignment doc in the wiki and started work on it.

## Date: 14/09/2026

- Firmare and Controls raised a good point that we have no way to calculate yaw (Absolute Heading), started looking into that. 

- Two main options to do it, First - Dual RTK GNSS setup and use carrier phase difference to calculate yaw, Second - More traditional method using a magnetometer to sense earth's magnetic field and calculating yaw.

- Started discussion on NTRIP vs base receiver for RTK error data with firmware, likely going with NTRIP.

## Date: 15/09/2026

- I have decided on a Board to Board connector, BB51-84AT-1-3HB 84 Pin - 1mm pitch, keyed, only about 1.5 CAD and decent stock in LCSC. Pin Assignment mostly done as well.

- Added Pin Assignment document to wiki with preliminary pin assignment.

- Firmware going with NTRIP and some free service for RTCM packets.

## Date: 19/09/2026

- Finished IMU schematics, going with ICM-40609-D for now. Firmware and Controls happy with it. Will confirm with shadab tomorrow

- RTK schematic, all the config is done. RF front end yet to be finished.

- Some progress on radio schematics.

## Date: 20/09/2026

- Added mezzanine connector (BB51-84AT-1-3HB) to altium library, unfortunately no CAD model available for socket.

- Finished the mezzanine schematic according to the pin assignment doc, made a few changes to pin assingment just asthetic stuff.

- Talked to alex about sensors on present FC, he agrees that sensors are pretty good nothing wrong with the hardware or model, likely driver mistake or we never tested it properly.

- Going with BMP585 for baro, latest from bosch, Gell Filled, more than enough pressure range, should give resolution around 0.5m or so, only 3.6 CAD and good stock in LCSC. Confirmed with Shadab as well.

- After a lot of discussions with shadab about budget and present FC, we decided that split from present FC is TVR takes the MCU's, COTS takes the BMI IMU's, all the GNSS and baro and TVR takes the ICM IMU's and TTL.

- Added FDCAN to pin assignment for future linear actuator plan.

- Still need to add VBAT in pin assignment.


