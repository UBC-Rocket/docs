# Design_Sync_04-10-26 Notes

## Issues Last Year

- Not enough time for proper testing and validation 

**Solution:** Quarterly design plan with design freeze date at least before 1.5 months


- Sensor selection did not cover all the parameter data needed by controls like baro's resolution is too low to detect takeoff and landing, GNSS resolution too low for 3D mapping etc.

**Solution:** LiDAR to detect takeoff and landing, RTK-GNSS for high resolution 3D surveying and UWB for indoor 3D Mapping


- Almost no sensor validation

**Solution:** Firmware to do IMU and Baro raw data validation as soon as possible, MAG noise characterisation as well.


- Didn’t measure accurate gimbal backlash (eye balled it)

**Solution:** TVR Mech to measure backlash of dynamixel - direct drive and indirect drive


- No variable thrust control yet, just static

**Solution:** Differential thrust control will be implemented, active controlled fins explored as well


- Horizontal Battery mounting kept on shifting drone's COM

**Solution:** Vertical battery mounting, aligned across central axis


- We lost lots of points for lack of validation / documentation (justifying decisions)

**Solution:** Wiki is set up and being actively used by TVR

## Hardware

### Backplane:

- Veritcal connector placement on bottom seems to be okay with all

- Considering sticking to parallel battery connectors until custom ESC is ready, alternative is a simple PDB (Under consideration, not a bad idea)

- Design freeze date for backplane is 18/05/26

### Flight Controller

- Sensor raw data validation is top priority

- Antenna interferance was raised, 915Mhz and GNSS Antenna is placed as far way as possible for some isolation

- Refer RC drones / open source designs for sensor selection, especially heading

- Consider optical flow sensor for position hold (Considering for phase 2)

- Add a SD card

### Battery:

- Tabless cells can give us max 12C which is exactly what we need, so no margin. Still under consideration but likely sticking to existing lipo

- Looking into hot swappable lipo, mech enclosure for our existing lipo to make hot swapping easier

- no conclusion on 12s lipo discussion

## TVR Mech
