# UHF RFID Distance Estimation  
### Real-Time Embedded RSSI Stabilization on R200 UHF Hardware

---

## 🚀 Project Summary

This project implements a **multi-stage RSSI filtering pipeline**
to improve distance estimation stability in UHF RFID systems.

**Measured Improvements:**

- 🔹 **MAE reduced by 5.7%**
- 🔹 **RMSE reduced by 8.2%**
- 🔹 **Accuracy improved by +9.1%p**

---

## 🧠 System Architecture

![System Diagram](assets/images/system-block-diagram.png)

Pipeline:

Raw RSSI  
→ Threshold Filtering  
→ Sliding Window  
→ Statistical Outlier Rejection  
→ Adaptive Weighted Averaging  
→ Log-Distance Model  
→ Distance Inertia Filter  

---

## 📊 Filtering Effect

Example at 1.6m (spike suppression):

![RSSI Filtering](assets/images/rssi-filtering-effect.png)

---

## 📈 Quantitative Results

### Overall Performance

| Metric | Raw | Filtered |
|--------|------|----------|
| MAE | 0.283 m | 0.266 m |
| RMSE | 0.442 m | 0.410 m |
| Accuracy (≤0.10m) | 41.9% | 51.0% |

---

### Distance-wise Error Comparison

![MAE](assets/images/mae-comparison.png)

![RMSE](assets/images/rmse-comparison.png)

![Accuracy](assets/images/accuracy-comparison.png)

---

## 🔬 Technical Highlights

- Embedded implementation (firmware-level filtering)
- Multi-point regression-based path loss calibration
- Real-time sliding window statistical processing
- Distance-domain temporal stabilization
- Experimental validation using 300 samples × 10 positions

---

## 📁 Repository Structure

- `firmware/` → Embedded implementation  
- `docs/` → Experimental documentation  
- `data/` → Raw & processed measurement data  
- `assets/` → Graphs and diagrams  

---

## 📌 License

MIT License
