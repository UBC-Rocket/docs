# ML AI Project Log

## DATE: October 18th, 2026

Project: Replace the current Kalman Filter used to process signals and create predictions with a custom ML fine-tuned to handle the specific noisy data from the rocket.
Work with the Firmware team to understand the current system, how data is being collected and how signals are being sent. What sort of processing is currently being done on signals and how can ML improve it? Also be mentored on using C.

[Kalman Filter](https://kalmanfilter.net/)

- Predicts state of system in the presence of noise
- Uses a dynamic model which describes the behavior of the system
- Noise is introduced as measurement noise and process noise which comes from the dynamic model
- Kalman Gain determines how much weight is given to the measurement verses the prediction to minimize the uncertainty of the new estimate
