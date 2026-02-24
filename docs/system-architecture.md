# System Architecture

## 1. System Overview

The system estimates the distance between a UHF RFID reader and a passive tag
using RSSI measurements obtained from an R200 UHF RFID reader.

The architecture is composed of four major processing stages:

1. RSSI Acquisition  
2. RSSI Domain Filtering  
3. Distance Estimation (Log-Distance Model)  
4. Distance Domain Stabilization  

Overall Processing Flow:

RFID Tag  
→ RSSI Acquisition  
→ RSSI Filtering  
→ RSSI-to-Distance Conversion  
→ Distance Inertia Filtering  
→ Final Distance Estimate  

---

## 2. Hardware Configuration

- RFID Reader: R200  
- Antenna: 5 dBi  
- RSSI Output Unit: dBm  
- Measurement Distance Range: 0.1 m – 2.0 m  
- Samples per Position: 300  
- Initial Samples Ignored: 5  
- Communication: Serial (115200 baud)  
- Controller: Arduino-compatible MCU  

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

The filtering architecture is divided into two domains:

- RSSI Domain → Noise suppression before nonlinear transformation  
- Distance Domain → Temporal stabilization after model conversion  

---

## 4. RSSI Domain Filtering

The first stage focuses on stabilizing RSSI measurements before
distance model conversion.

### 4.1 Physical Threshold Filtering

RSSI values outside the physically valid range:

-85 dBm ≤ RSSI ≤ -35 dBm

are rejected to remove abnormal or corrupted readings.

---

### 4.2 Sliding Window Processing

A sliding window of 5 samples is maintained.

This enables:

- Short-term smoothing  
- Local statistical analysis  
- Reduced high-frequency noise  

---

### 4.3 Statistical Outlier Rejection

For each window:

- Mean and standard deviation are computed  
- Samples outside (mean ± 1.8 × standard deviation) are discarded  

This suppresses spike noise caused by multipath reflection
and environmental RF interference.

---

### 4.4 Adaptive Mixing

Two averages are computed:

1. Simple moving average  
2. Freshness-weighted average  

Final filtered RSSI:

Filtered RSSI =
(1 − 0.4) × Moving Average +
0.4 × Weighted Average

This improves responsiveness while maintaining stability.

---

## 5. Distance Estimation Model

The log-distance path loss model is applied:

RSSI = P0 − 10n log10(d)

Rearranged for distance estimation:

d = 10^((P0 − RSSI) / (10n))

Where:

- P0 = reference RSSI at 1 m  
- n = path-loss exponent  
- RSSI in dBm  
- d in meters  

Estimated parameters (experimentally derived):

- P0 = -64.91 dBm  
- n = 1.769  

These parameters were obtained using on-device calibration firmware.

---

## 6. Distance Domain Inertia Filtering

After RSSI-to-distance conversion, a temporal stabilization filter is applied:

Filtered Distance =
α × Previous Distance +
(1 − α) × Current Distance

A position-dependent inertia factor (α) is used to:

- Reduce sudden estimation jumps  
- Improve temporal stability  
- Suppress residual fluctuation after nonlinear conversion  

This second-stage filtering significantly improves practical usability
in indoor RF environments.

---

## 7. Design Objective

The proposed multi-stage architecture aims to:

- Suppress RSSI spike noise  
- Improve estimation stability  
- Reduce MAE and RMSE  
- Increase accuracy within ±0.10 m  
- Improve robustness under indoor multipath conditions  

The separation of filtering into RSSI-domain and distance-domain
processing enhances overall system reliability.
