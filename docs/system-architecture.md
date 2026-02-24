# System Architecture

## 1. System Overview

The system estimates tag-to-reader distance using RSSI values obtained from a UHF RFID R200 reader.

The architecture consists of four processing stages:

1. RSSI Acquisition  
2. RSSI Domain Filtering  
3. Distance Estimation  
4. Distance Domain Stabilization  

---

## 2. Hardware Configuration

- RFID Reader: R200
- Antenna: 5dBi
- RSSI Output Unit: dBm
- Measurement Distance Range: 0.1m – 2.0m
- Samples per Position: 300
- Initial Samples Ignored: 5

RSSI values are collected via serial communication and processed in real time.

---

## 3. Signal Processing Flow

Raw RSSI (dBm)
        ↓
Physical Threshold Filtering
        ↓
Sliding Window Buffer (size = 5)
        ↓
Statistical Outlier Removal
        ↓
Adaptive Weighted Averaging
        ↓
RSSI-to-Distance Conversion
        ↓
Distance Domain Inertia Filtering
        ↓
Final Distance Estimate

---

## 4. RSSI Domain Filtering

### 4.1 Physical Threshold Filtering

RSSI values outside the range:

-85 dBm ≤ RSSI ≤ -35 dBm

are rejected to eliminate physically invalid readings.

---

### 4.2 Sliding Window Processing

A window of 5 samples is maintained.

This enables short-term noise smoothing and local statistical analysis.

---

### 4.3 Statistical Outlier Rejection

For each window:

- Mean and standard deviation are computed.
- Samples outside (mean ± 1.8 × standard deviation) are discarded.

This suppresses spike noise caused by RF reflections and interference.

---

### 4.4 Adaptive Mixing

Two averages are computed:

1. Simple moving average
2. Freshness-weighted average

Final filtered RSSI:

Filtered RSSI =
(1 − 0.4) × Moving Average +
0.4 × Weighted Average

---

## 5. Distance Estimation Model

The log-distance path loss model is applied:

d = 10^((P0 − RSSI) / (10n))

Where:

- P0 = -64.91 dBm
- n = 1.769
- RSSI in dBm
- d in meters

Model parameters were derived from experimental measurements.

---

## 6. Distance Domain Inertia Filtering

After RSSI conversion:

Filtered Distance =
α × Previous Distance +
(1 − α) × Current Distance

A position-dependent inertia factor (α) is applied to:

- Reduce fluctuation
- Improve temporal stability
- Suppress residual estimation noise

---

## 7. Design Objective

- Suppress RSSI spike noise
- Improve estimation stability
- Reduce MAE and RMSE
- Increase accuracy within ±0.10m
