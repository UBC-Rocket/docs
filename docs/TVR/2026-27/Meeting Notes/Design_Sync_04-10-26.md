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

- Add a SD card to store raw sensor data

- Design freeze date for flight controller is 18/05/26

### Battery:

- Tabless cells can give us max 12C which is exactly what we need, so no margin. Still under consideration but likely sticking to existing lipo

- Looking into hot swappable lipo, mech enclosure for our existing lipo to make hot swapping easier

- no conclusion on 12s lipo discussion

## TVR Mech
## Roll Control System

- Action item proposed by Dr. Maddison: consider a roll control system

- Disposition is to not deal with it during this design due to the limited mass budget, but look into it for a future design

## Linear Actuator Research

- Gugan needs to research a properly specified linear actuator

- Check understanding regarding board space for the linear actuators

## GitHub

- Have a ticket request system to grant people access to repos and CAD files for editing

- Add proper checks for assemblies

- People need to get permission to make changes

- Have a single frozen design

- Have edits branch off of the frozen design, therefore edits are only ever changing the branch off of the frozen design

- After the dev design has been — **[Note incomplete]**

- Add people who have admin access as reviewers and allow reviewers to have admin access

- Add tags

- Work with Jason on version-related control

## Current Design

### Backlash

- Backlash is the biggest problem

- Need to measure backlash

- Testing method: measure backlash using a pen and grid

### Indirect

- Higher gear ratio

- Need to measure backlash

### Direct

- Attach directly and then move components to rectify the center of mass

- Frame would possibly need to expand

- Needs a design

- Need to remeasure backlash

- Add a "dead mass" to help move the center of mass

### Battery Tangent

- Decrease

### Thrust

- Need more thrust ☹️

## Next Week Goal

### New Gimbal

- Higher gear ratio to reduce backlash

- Buy gears (McMaster-Carr)

- Print resin gears

- Direct Drive


### New Legs

- New leg mounts to main frame, just screw it in, dampening system can be explored later on. Look at Open Source Projects.

## Other Current Design Notes

- There is wiggle in that one screw component that breaks (Shoulder bolt holding the lower seervo mount)

- Look into thrust-vectoring drones that solve this given problem

- Need more external research into solved servo-driven designs:
  - What have people done?
  - Why has it worked?
  - How can we copy and integrate it?

## Leg System

- Have we looked at other people's designs?

- Do we have more leg designs?

- Get more leg designs underway (**urgent**)

- More consideration of stress concentrations in the next design

## Batteries

- Stop plate

- Move away from horizontal batteries

- Look at open-source models to save ourselves the trouble

- Get a better design than the Velcro strap

- Look at rubber on nylon nuts to stop vertical movement
  
- COM should not change after each battery swap

## Important

- **NEED TO MEASURE THE BACKLASH**

- A quick way would be to attach a pencil to the thrust cage and, while the servos are locked, attempt to move the servo. The pencil will mark how much backlash is present, which we can then measure
