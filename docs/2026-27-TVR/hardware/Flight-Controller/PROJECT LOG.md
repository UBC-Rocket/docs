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

- Firmware confirmed that they are transmiting approx 3.5Kbps now and with this year's design, it may go up to 5Kbps max. So no wories there.

- Finalized Radio specs - 915MHz, 300Kbps, GFSK-FHSS, 30dBm Transmit Power (External PA), approx 140dBm receiver senstivity, Antenna Diversity and standard 2dBi antenna.

- 915MHz ISM band transmit power limit is 30dBm and EIRP of 36 dBm.

- *Still need to figure FHSS details, no. of channel and bandwidth.

- Firmware going with NTRIP and some free service for RTCM packets.

## Date: 18/09/2026

- Went down a rabbit hole and had a scare about data rates and transmit power of RFD, comparison with SX1262 and looked at alternative IC's but all good. Just was misunderstanding things.

- Started on a high level block diagram for custom telemetry front end, will take my time with and calculate/simulate everything.

## Date: 19/09/2026

- Finished IMU schematics, going with ICM-40609-D for now. Firmware and Controls happy with it. Will confirm with shadab tomorrow

- RTK schematic, all the config is done. RF front end yet to be finished.

- Good progress on RF front end block diagram, need to iron out a few more high level details and then can start deepdives on each.

- **Important:** fundamentally, Bias Tee L needs have sufficiently high impedence and C needs to have sufficiently low impedence at signal frequency. The SRF peak does not need to right at signal frequency or even very close to it. Bias tee L and C needs to speced so that their SRF is comfortablly above signal frequency especially L can be quite above it as L still gives good impedence at either side of SRF peak until a decent bandwidth. Remember, passive size is a big factor in parasitics. 0402 is standard for RF front cause parasitics are nice and can usually find values required. For Sub-GHz, 0603 can work as well but **SPEC IT PROPERLY**.

- Going with 0603 as much as i can for RF front end, will be careful

- After getting confused and trying a lot of combinations, finalized Bias Tee values. Schematic updated with Bias Tee.

## Date: 20/09/2026

- Added mezzanine connector (BB51-84AT-1-3HB) to altium library, unfortunately no CAD model available for socket.

- Finished the mezzanine schematic according to the pin assignment doc, made a few changes to pin assingment just asthetic stuff.

- Talked to alex about sensors on present FC, he agrees that sensors are pretty good nothing wrong with the hardware or model, likely driver mistake or we never tested it properly.

- Going with BMP585 for baro, latest from bosch, Gell Filled, more than enough pressure range, should give resolution around 0.5m or so, only 3.6 CAD and good stock in LCSC. Confirmed with Shadab as well.

- After a lot of discussions with shadab about budget and present FC, we decided that split from present FC is TVR takes the MCU's, COTS takes the BMI IMU's, all the GNSS and baro and TVR takes the ICM IMU's and TTL.

- Added FDCAN to pin assignment for future linear actuator plan.

- Still need to add VBAT in pin assignment.

## Date: 22/09/2026

- Added VBAT to pin assingment doc, very little pins left. May need to move things around.

- Revised matching circuits, L / T / Pi circuits. Seems like L gives us lowest / fixed Q-factor but widest bandwidth and easiest to implement, Pi gives ability to control Q-factor with the extra capacitor and generally used for high impedence matching (>100R) and T gives the highest Q-factor, ability to control Q and narrow bandwidth, generaly used for high quality low impedence matching (<100R).

- **Important:** General implementation of T will AC couple the RF line, so will not work for active antenna's. 

- Decided on L matching for everything except pi for antenna matching, Semtech seems to recommend the same.