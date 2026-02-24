# UHF RFID Distance Estimation

## Overview

Embedded real-time distance estimation system using R200 UHF RFID hardware.

Multi-stage RSSI stabilization pipeline:

- Physical threshold filtering
- Statistical outlier rejection
- Adaptive weighted averaging
- Distance-domain inertia filtering

---

## System Architecture

![System Diagram](assets/images/Schematic.JPG)

---

## Performance Improvement

| Metric | Raw | Filtered |
|--------|------|----------|
| MAE | 0.283 m | 0.266 m |
| RMSE | 0.442 m | 0.410 m |
| Accuracy (≤0.10m) | 41.9% | 51.0% |

---

## Filtering Effect Example

![RSSI Filtering](assets/images/rssi-filtering-effect.png)

---

## Distance-wise Error Comparison

![MAE](assets/images/mae-comparison.png)
![RMSE](assets/images/rmse-comparison.png)
![Accuracy](assets/images/accuracy-comparison.png)

---

## Key Contributions

- Embedded multi-stage RSSI stabilization
- Statistical outlier rejection
- Adaptive weighted averaging
- Distance-domain inertia filtering
- Quantitative performance validation
