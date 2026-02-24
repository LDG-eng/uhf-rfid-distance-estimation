# Experiment Setup

## Objective

To evaluate RSSI-based distance estimation accuracy
and validate the effectiveness of the proposed filtering algorithm.

---

## Hardware Configuration

- RFID Reader: R200
- Antenna: 5 dBi
- Operating Frequency: UHF band
- RSSI Unit: dBm
- Microcontroller: Arduino-compatible board
- Communication: SoftwareSerial (115200 baud)

---

## Measurement Environment

- Indoor environment
- Line-of-sight (LOS) condition
- Minimal human movement during measurement
- Tag fixed on non-metallic surface

---

## Measurement Procedure

1. Place the RFID tag at predefined distances.
2. Measurement range: 0.1 m to 2.0 m
3. Step interval: 0.1 m
4. Collect 300 RSSI samples per position.
5. Ignore first 5 samples (stabilization phase).
6. Apply RSSI filtering pipeline.
7. Convert RSSI to distance using log-distance model.
8. Apply distance-domain inertia filtering.

---

## Calibration Procedure

To derive the log-distance path loss model parameters (P0 and n),
RSSI measurements were collected at multiple known distances.

### Measurement Positions

RSSI samples were collected at the following distances (meters):

0.1, 0.3, 0.5, 0.7, 0.9,
1.1, 1.3, 1.5, 1.7, 2.0

### Sampling Configuration

- Samples per position: 300
- Initial samples discarded: 5

### Parameter Estimation Method

For each distance, the average RSSI value was computed after
discarding the initial unstable samples.

Using the log-distance path loss model:

RSSI(d) = P0 − 10n log10(d)

Linear regression was performed on:

log10(d) vs RSSI(d)

to estimate:

- P0 (reference power)
- n (path-loss exponent)

The final model parameters used in this study were:

P0 = -64.91 dBm  
n = 1.769

---

## Accuracy Metric

Accuracy criterion:

±0.10 m from ground-truth distance

Performance metrics:

- MAE (Mean Absolute Error)
- RMSE (Root Mean Square Error)
- Accuracy (within ±0.10 m)
