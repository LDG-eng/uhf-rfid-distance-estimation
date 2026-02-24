# UHF RFID Distance Estimation System
UHF RFID RSSI-based distance estimation system with noise reduction filtering

## Overview
This project implements an RSSI-based distance estimation system using a UHF RFID R200 reader and a 5dBi antenna.  
A multi-stage hybrid filtering algorithm was designed and implemented on an embedded system to improve distance estimation stability under noisy RF conditions.

---

## System Configuration

- RFID Reader: R200
- Antenna: 5dBi
- RSSI Unit: dBm
- Measurement Range: 0.1m – 2.0m
- Samples per Position: 300
- Initial Samples Ignored: 5
- Accuracy Criterion: ±0.10m

Distance model parameters:

- Reference Power (P0): -64.91 dBm
- Path-loss Exponent (n): 1.769

---

## Filtering Architecture

### 1) RSSI Domain Stabilization

- Physical threshold filtering  
  (-85 dBm ≤ RSSI ≤ -35 dBm)
- Sliding window size: 5
- Statistical outlier rejection  
  (mean ± 1.8 × standard deviation)
- Adaptive mixing:
  - Simple moving average
  - Freshness-weighted average  
  (mix ratio = 0.4)

### 2) Distance Domain Inertia Filtering

After RSSI-to-distance conversion, a position-dependent inertia factor was applied:

Filtered Distance =  
α × Previous Distance + (1 − α) × Current Estimate

This improves estimation stability in high-variance regions.

---

## Distance Estimation Model

Log-distance path loss model:

d = 10^((P0 − RSSI) / (10n))

Where:
- P0 = -64.91 dBm
- n = 1.769
- RSSI in dBm
- d in meters

---

## Experimental Results

| Metric | Raw | Filtered |
|--------|------|----------|
| MAE | 0.283 m | 0.266 m |
| RMSE | 0.442 m | 0.410 m |
| Accuracy (≤0.10m) | 41.9% | 51.0% |

### Performance Improvement

- MAE reduced by 5.7%
- RMSE reduced by 8.2%
- Accuracy improved by +9.1%p

---

## Repository Structure

firmware/ → Embedded RSSI processing code
data/ → Experimental measurement data
docs/ → System architecture and experiment documentation

---

## Key Contributions

- Embedded multi-stage RSSI stabilization
- Statistical outlier-based filtering
- Adaptive weighted averaging
- Distance-domain inertia compensation
- Quantitative performance validation

---

## Visualization

### System Block Diagram
![System Diagram](assets/images/Schematic.png)

### RSSI Filtering Effect
![RSSI Filtering](assets/images/rssi-filtering-effect.png)

### Distance Estimation Comparison
![Distance Comparison](assets/images/distance-estimation-comparison.png)

### Error Metrics by Distance
![MAE](assets/images/mae-comparison.png)
![RMSE](assets/images/rmse-comparison.png)
![Accuracy](assets/images/accuracy-comparison.png)
