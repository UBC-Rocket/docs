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

## Date: 15/09/2026

- I have decided on a Board to Board connector, BB51-84AT-1-3HB 84 Pin - 1mm pitch, keyed, only about 1.5 CAD and decent stock in LCSC. Pin Assignment mostly done as well.

- Added Pin Assignment document to wiki with preliminary pin assignment





