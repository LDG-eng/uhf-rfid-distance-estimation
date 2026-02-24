# Model Parameter Estimation

## Purpose

This script estimates the log-distance path loss model parameters (P0 and n) using experimentally measured RSSI values at known distances.

---

## Model

Log-distance path loss model:

d = 10^((P0 − RSSI) / (10n))

Rearranged form:

RSSI = P0 − 10n log10(d)

This linear form enables parameter estimation using measured (distance, RSSI) pairs.

---

## Method

1. Measure RSSI at predefined distances.
2. Compute log10(d).
3. Fit the linear model:
   RSSI = P0 − 10n log10(d)
4. Extract:
   - P0 (intercept)
   - n (derived from slope)

---

## Result

Estimated parameters:

- P0 = -64.91 dBm
- n = 1.769

These values are used in the firmware for real-time distance estimation.
