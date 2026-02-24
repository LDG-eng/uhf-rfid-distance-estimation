# Arduino RSSI Processing Firmware

## Overview

This firmware collects RSSI data from the R200 UHF RFID reader via serial communication and performs real-time distance estimation.

The implementation includes multi-stage RSSI filtering and distance-domain stabilization.

---

## Core Functions

1. RSSI acquisition via serial interface
2. Physical threshold filtering (-85 dBm to -35 dBm)
3. Sliding window buffering (size = 5)
4. Statistical outlier rejection (mean ± 1.8σ)
5. Adaptive weighted averaging (mix ratio = 0.4)
6. Log-distance path loss conversion
7. Distance-domain inertia filtering
8. Real-time performance metric calculation (MAE, RMSE, Accuracy)

---

## Key Parameters

- WINDOW_SIZE = 5
- OUTLIER_K = 1.8
- MIX_RATIO = 0.4
- SAMPLES_PER_POSITION = 300
- SAMPLES_TO_IGNORE = 5
- P0 = -64.91
- n = 1.769

---

## Processing Flow

Raw RSSI  
→ Threshold Filtering  
→ Sliding Window  
→ Outlier Removal  
→ Adaptive Averaging  
→ Distance Conversion  
→ Inertia Filtering  
→ Metric Evaluation

---

## Implementation Notes

- Window is pre-initialized to stabilize early estimates.
- Initial unstable samples are excluded from evaluation.
- Both raw and filtered results are computed for comparison.
