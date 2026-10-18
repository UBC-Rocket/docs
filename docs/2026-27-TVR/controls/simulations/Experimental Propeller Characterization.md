Introduction

To determine the details of the propeller characterization experiment. This document outlines the meeting between the mechanical team and the simulations team.

Main Body

# Big Picture

- Characterize the throttle values required to achieve desired thrust and torque values achieved.
- This characterization will be a vital part of the control system.

 
# Details

- The experiment will be conducted on the thrust stand built by the mechanical team.

- The motors will be operated with throttle values expressed in percentages(not RPM or PWM signals).

- Values will be recorded in increments in an array from 0 throttle to max throttle.

- The proposed increments for or initial proof of concept and phase 1 was 5%.
  - (Max throttle values and the highest resolution for the increments must be determined)
- Automation of this experiment is vital as it will be repeated with each version of the system during future phases.
  - A python script interfacing with an Arduino was proposed.
  - Although the python script is possible to work with, interfacing directly with the Arduino was proposed to be simpler and more efficient.
  - Using Arduino Libraries to save csv files and extracting them was proposed to be simpler. 
  - The script must have initial and final thrust values and desired increment as inputs, and must output a formatted csv file ready for python processing.

 
# Shortcomings
- Battery Drain during experiment
  - The battery used may drain during the experiment, skewing data.
  - Using a power supply during the experiment was proposed, but the available power supply was inadequate for the task.
  - Solution: Source a power supply capable for the job.

- Max thrust may not be achieved

  - Because of the geometry of the thrust cage and the orientations of the motors, the magnitudes of the torques generated at max throttle may not be equal. This would limit the maximum throttle available for the rocket.
     Solution: Determine the effective maximum throttle in which the torque values match. This maximum throttle may be different for each motor.
