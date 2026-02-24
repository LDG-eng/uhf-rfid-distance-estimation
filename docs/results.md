# Experimental Results

## Evaluation Overview

The performance of the RSSI-based distance estimation system
was evaluated before and after applying the proposed filtering architecture.

Evaluation range:

0.1 m – 2.0 m  
Step interval: 0.1 m  
Samples per position: 300  
Accuracy criterion: ±0.10 m  

---

## Error Metrics

| Metric | Raw RSSI | Filtered |
|--------|----------|----------|
| MAE | 0.283 m | 0.266 m |
| RMSE | 0.442 m | 0.410 m |
| Accuracy (≤ 0.10 m) | 41.9% | 51.0% |

---

## Improvement Analysis

### MAE Reduction
0.283 → 0.266 m  
Reduction: **5.7%**

### RMSE Reduction
0.442 → 0.410 m  
Reduction: **8.2%**

### Accuracy Improvement
41.9% → 51.0%  
Improvement: **+9.1 percentage points**

---

## Interpretation

- Multi-stage RSSI filtering reduced high-variance noise.
- Statistical outlier rejection improved signal stability.
- Distance-domain inertia filtering reduced sudden estimation jumps.
- Overall distance estimation reliability improved across the full range.

The proposed filtering structure demonstrates measurable
accuracy improvement under real indoor RF conditions.
