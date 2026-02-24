# Experiment Setup

## 1. Objective

To evaluate the effectiveness of the proposed multi-stage hybrid filtering algorithm in improving RSSI-based distance estimation accuracy.

The performance comparison was conducted between:

- Raw RSSI-based distance estimation
- Filtered RSSI-based distance estimation

---

## 2. Experimental Environment

- RFID Reader: R200
- Antenna: 5dBi
- RSSI Unit: dBm
- Environment: Indoor environment (line-of-sight condition)
- Measurement Range: 0.1m – 2.0m

Distance measurement points:

0.1m, 0.4m, 0.7m, 1.0m, 1.3m, 1.6m, 2.0m

---

## 3. Data Collection Method

- Samples per distance: 300
- Initial unstable samples ignored: 5

RSSI values were continuously collected via serial communication and processed in real time.

Each distance position was evaluated independently.

---

## 4. Distance Estimation Process

For each sample:

1. Raw RSSI → Direct distance conversion (log-distance model)
2. Filtered RSSI → Hybrid filtering → Distance conversion
3. Distance-domain inertia applied (filtered case only)

Both results were recorded for performance comparison.

---

## 5. Performance Metrics

The following metrics were used:

### 5.1 MAE (Mean Absolute Error)

MAE = mean(|estimated distance − actual distance|)

Indicates average estimation error magnitude.

---

### 5.2 RMSE (Root Mean Square Error)

RMSE = sqrt(mean((estimated distance − actual distance)^2))

Sensitive to large error spikes.

---

### 5.3 Accuracy (≤ 0.10m)

Accuracy (%) =
(Number of samples with |error| ≤ 0.10m / Total samples) × 100

This metric evaluates practical positioning reliability.

---

## 6. Evaluation Strategy

Performance was compared between:

- Raw estimation
- Filtered estimation

Improvement was evaluated using:

- MAE reduction rate
- RMSE reduction rate
- Accuracy increase (percentage point gain)

---

## 7. Key Evaluation Focus

- Stability improvement under RSSI fluctuation
- Spike noise suppression effectiveness
- Improvement in short-range precision
- Overall estimation reliability within ±0.10m
