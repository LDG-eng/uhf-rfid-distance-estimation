# UHF RFID Distance Estimation System
UHF RFID RSSI-based distance estimation system with noise reduction filtering

## Overview
This project implements a UHF RFID RSSI-based distance estimation system using an R200 reader module with a 5dBi antenna.

The objective is to improve distance estimation accuracy under RSSI noise conditions by applying a hybrid adaptive moving average filter.

---

## System Configuration

- RFID Module: R200
- Antenna: 5dBi
- RSSI Unit: dBm
- Measurement Range: 0.1m to 2.0m
- Distance Model Parameters:
  - P0 = -64.91
  - Path-loss exponent (n) = 1.769

---

## Signal Processing Approach

A hybrid adaptive moving average filter was implemented with the following characteristics:

- Statistical threshold filtering using mean and standard deviation
- Outlier rejection based on standard deviation
- Freshness-weighted averaging
- Adaptive smoothing to suppress spike noise

The filter combines statistical outlier detection with weighted averaging to improve RSSI stability before distance estimation.

---

## Performance Metrics

Accuracy criterion: within ±0.10m error

### Overall Performance

| Metric | Raw | Filtered |
|--------|------|----------|
| MAE | 0.283m | 0.266m |
| RMSE | 0.442m | 0.410m |
| Accuracy (≤0.10m) | 41.9% | 51.0% |

---

### Improvement Summary

- MAE reduced by approximately 5.7%
- RMSE reduced by approximately 8.2%
- Accuracy (≤0.10m) improved by approximately 9.1%p

The larger reduction in RMSE indicates effective suppression of extreme RSSI fluctuations and spike noise.

---

## Key Contribution

- Custom hybrid adaptive filtering algorithm design
- Statistical RSSI stabilization
- Quantitative validation using MAE, RMSE, and accuracy metrics
- MATLAB-based performance analysis
