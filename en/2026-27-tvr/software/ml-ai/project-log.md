---
title: 'ML AI Project Log'
description: ''
published: true
date: '2026-11-08T22:22:42.000Z'
tags: []
editor: markdown
dateCreated: '2026-11-08T22:22:42.000Z'
---

## DATE: October 18th, 2026

Project: Replace the current Kalman Filter used to process signals and create predictions with a custom ML fine-tuned to handle the specific noisy data from the rocket.
Work with the Firmware team to understand the current system, how data is being collected and how signals are being sent. What sort of processing is currently being done on signals and how can ML improve it? Also be mentored on using C.

[Kalman Filter](https://kalmanfilter.net/)

- Predicts state of system in the presence of noise
- Uses a dynamic model which describes the behavior of the system
- Noise is introduced as measurement noise and process noise which comes from the dynamic model
- Kalman Gain determines how much weight is given to the measurement verses the prediction to minimize the uncertainty of the new estimate

## DATE: October 25th, 2026

### Kalman Filter's with ML

[KalmanNet](https://arxiv.org/pdf/2107.10043)

Hybrid framework combining KF with RNN. Calculation of Kalman gain delegated to RNN.
Best for non-linear systems where the state space is not fully known.

[Kalman Filter with ML](https://www.sciencedirect.com/science/article/pii/S1367578823000731)

Kalman filters face two main problems: system model and noise model parameter identification. Machine learning can be used to determine these parameters. Hybrid ML and Kalman filters show results in higher accuracy and reliability.

Machine learning can be used to determine the state equations of the system based on experimental data when the dynamics of a system are hard to describe.

Machine learning can be used to estimate the parameters of a Kalman filter, such as determining the noise model parameters.

Use gated recurrent unit (GRN), a type of recurrent neural network (RNN), because it is light weight. Best used for predicting error.

[GRU-aided ESKF](https://www.mdpi.com/2226-4310/10/11/923)

Used a GRU-aided ESKF (error-state Kalman filter) for visual inertial odometry in this paper. Showed significant error reduction of up to 78% compared to filter that did not utilize ML. The GRU was used to predict error position error.

[ESKF](https://www.sciencedirect.com/science/article/pii/S0967066123001727)

More info on ESKF. Good for nonlinear applications to accurately estimate error. Used in tandem with EKF (extended Kalman filter).

Using a small model it could be possible to run the ML-aided KF on a single M7 core with under 1MB of RAM and less than 250 kB of flash memory. Might be worth just adding a another board to the stack as it would provide significantly more compute and allow for a larger ML model.

## DATE: November 1st, 2026

### Kalman Filter's with ML (cont)

[RTN MPC](https://arxiv.org/pdf/2203.07747)

Use ML to model complex dynamics system. Uses gradient-based online optimization MPC to run large models with real-time results.Reduced positional tracking error by up to 82%.

Start with a lightweight dynamics model for the model to build off of, and have the model predict only residuals so a smaller model can be used. Use native machine learning batch differentiation (built into PyTorch or TensorFlow) to compute derivatives in parallel. Locally linearize the network graph at each timestep.

[Colored Noise](https://www.mdpi.com/2073-8994/18/10/1669)
[CKFNet](https://arxiv.org/pdf/2508.09727)

Hybrid framework, integrating GRUs with the KF to eliminate the need for Kalman Gain computations and explicit noise modelling and instead have the GRU model the noise. Works well for application where noise is colored and varies with time.

CKFNet uses 151k parameters, enabling real-time deployment. More efficient than KalmanNet.
