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

Before distance estimation:

1. Measure average RSSI at 1.0 m → determine P0
2. Measure average RSSI at 3.0 m → compute path-loss exponent n

Model:

RSSI = P0 − 10n log10(d)

Estimated parameters:

- P0 = -64.91 dBm
- n = 1.769

---

## Accuracy Metric

Accuracy criterion:

±0.10 m from ground-truth distance

Performance metrics:

- MAE (Mean Absolute Error)
- RMSE (Root Mean Square Error)
- Accuracy (within ±0.10 m)
