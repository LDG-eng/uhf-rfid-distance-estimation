# Experimental Results

## 1. Evaluation Overview

The RSSI-based distance estimation system was evaluated
before and after applying the proposed multi-stage filtering architecture.

Measurement range:
0.1 m – 2.0 m  
Step interval: 0.1 m  
Samples per position: 300  
Accuracy criterion: ±0.10 m  

Performance was evaluated using:

- MAE (Mean Absolute Error)
- RMSE (Root Mean Square Error)
- Accuracy (within ±0.10 m)

---

## 2. Overall Performance Comparison

| Metric | Raw | Filtered |
|--------|------|----------|
| MAE | 0.283 m | 0.266 m |
| RMSE | 0.442 m | 0.410 m |
| Accuracy (≤ 0.10 m) | 41.9% | 51.0% |

### Improvement Summary

- MAE reduced by **5.7%**
- RMSE reduced by **8.2%**
- Accuracy improved by **+9.1 percentage points**

Filtered estimation consistently outperformed
raw RSSI-based estimation across overall metrics.

---

## 3. Distance-wise Performance

| Distance | Raw MAE | Filt MAE | Raw RMSE | Filt RMSE | Raw Acc (%) | Filt Acc (%) |
|-----------|---------|----------|-----------|------------|--------------|---------------|
| 0.1m | 0.014 | 0.013 | 0.018 | 0.013 | 99.7 | 100.0 |
| 0.4m | 0.071 | 0.069 | 0.076 | 0.072 | 94.3 | 97.7 |
| 0.7m | 0.082 | 0.078 | 0.094 | 0.084 | 91.7 | 89.7 |
| 1.0m | 0.234 | 0.225 | 0.288 | 0.235 | 1.7 | 3.0 |
| 1.3m | 0.842 | 0.811 | 0.924 | 0.854 | 1.3 | 1.7 |
| 1.6m | 0.152 | 0.085 | 0.182 | 0.110 | 0.0 | 64.3 |
| 2.0m | 0.589 | 0.585 | 0.620 | 0.604 | 4.3 | 0.7 |

---

## 4. Key Observations

- Short-range region (0.1 m – 0.7 m) shows high baseline accuracy.
- Significant accuracy improvement at 1.6 m (0.0% → 64.3%).
- RMSE reduction is larger than MAE reduction, indicating effective suppression of extreme RSSI spikes.
- Mid-range distances (1.0 m – 1.3 m) remain sensitive to RSSI fluctuation.
- Performance degradation at 2.0 m suggests increasing multipath and attenuation effects.

---

## 5. Interpretation

The proposed hybrid filtering strategy:

- Suppresses RSSI spike noise in the RSSI domain
- Reduces nonlinear amplification of noise during distance conversion
- Stabilizes output using distance-domain inertia filtering

The larger reduction in RMSE compared to MAE confirms that
extreme deviations were effectively mitigated.

Overall, the layered filtering architecture improves
practical indoor distance estimation reliability.


## Limitations

- Mid-range distances (1.0–1.3 m) show instability
- Multipath interference not explicitly modeled
- Environment-specific calibration required


## Future Work

- Adaptive path-loss exponent estimation
- Kalman filtering in distance domain
- Multi-antenna fusion
- Machine-learning-based RSSI regression
