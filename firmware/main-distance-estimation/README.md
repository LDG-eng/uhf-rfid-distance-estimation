# Arduino RSSI Distance Estimation Firmware

## Overview

This firmware collects RSSI data from the R200 UHF RFID reader and performs real-time distance estimation with a multi-stage hybrid filtering algorithm.

Both raw and filtered distance results are calculated for performance comparison.

---

## File Structure

rssi_distance_estimator.ino  
→ Main firmware implementing RSSI processing and evaluation logic

---

## Processing Pipeline

1. RSSI acquisition via serial interface  
2. Physical threshold filtering (-85 dBm to -35 dBm)  
3. Sliding window buffering (size = 5)  
4. Statistical outlier rejection (mean ± 1.8σ)  
5. Adaptive mixing (moving average + weighted average)  
6. Log-distance path loss conversion  
7. Distance-domain inertia filtering  
8. Real-time MAE, RMSE, and accuracy computation  

---

## Key Parameters

WINDOW_SIZE = 5  
OUTLIER_K = 1.8  
MIX_RATIO = 0.4  
SAMPLES_PER_POSITION = 300  
SAMPLES_TO_IGNORE = 5  

Distance model:

P0 = -64.91  
n = 1.769  

---

## Filtering Strategy

### RSSI Domain Filtering

- Reject invalid RSSI values
- Remove statistical outliers
- Apply adaptive averaging

### Distance Domain Filtering

Filtered Distance =  
α × Previous Distance + (1 − α) × Current Distance

A position-dependent inertia factor is applied to stabilize estimation.

---

## Notes

- Window is pre-initialized to reduce startup instability.
- First 5 samples are excluded from evaluation.
- Both raw and filtered performance metrics are calculated internally.
