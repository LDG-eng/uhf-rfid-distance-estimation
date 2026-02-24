UHF RFID Distance Estimation System

UHF RFID RSSI-based distance estimation system with noise reduction filtering

Overview

This project implements an RSSI-based distance estimation system using a UHF RFID R200 reader and a 5dBi antenna.
A multi-stage hybrid filtering algorithm was designed and implemented on an embedded system to improve distance estimation stability under noisy RF conditions.

System Configuration

RFID Reader: R200

Antenna: 5dBi

RSSI Unit: dBm

Measurement Range: 0.1 m – 2.0 m

Samples per Position: 300

Initial Samples Ignored: 5

Accuracy Criterion: ±0.10 m

Distance model parameters:

Reference Power 
𝑃
0
P
0
	​

: -64.91 dBm

Path-loss Exponent 
𝑛
n: 1.769

Filtering Architecture
1) RSSI Domain Stabilization

Physical threshold filtering: 
−
85
 dBm
≤
RSSI
≤
−
35
 dBm
−85 dBm≤RSSI≤−35 dBm

Sliding window size: 5

Statistical outlier rejection: 
mean
±
1.8
×
std
mean±1.8×std

Adaptive mixing:

Simple moving average (MA)

Freshness-weighted average (WMA)

Final filtered RSSI:

RSSI
filtered
=
(
1
−
0.4
)
⋅
MA
+
0.4
⋅
WMA
RSSI
filtered
	​

=(1−0.4)⋅MA+0.4⋅WMA
2) Distance Domain Inertia Filtering

After RSSI-to-distance conversion, a position-dependent inertia factor is applied:

𝐷
filtered
=
𝛼
⋅
𝐷
prev
+
(
1
−
𝛼
)
⋅
𝐷
current
D
filtered
	​

=α⋅D
prev
	​

+(1−α)⋅D
current
	​


This improves estimation stability in high-variance regions.

Distance Estimation Model

Log-distance path loss model:

𝑑
=
10
𝑃
0
−
RSSI
10
𝑛
d=10
10n
P
0
	​

−RSSI
	​


Where:

𝑃
0
=
−
64.91
 dBm
P
0
	​

=−64.91 dBm

𝑛
=
1.769
n=1.769

RSSI in dBm

d in meters

Model parameters were derived from experimental measurements.

Experimental Results
1. Overall Performance Comparison
Metric	Raw	Filtered
MAE	0.283 m	0.266 m
RMSE	0.442 m	0.410 m
Accuracy (≤0.10 m)	41.9%	51.0%

Improvement Summary

MAE reduced by 5.7%

RMSE reduced by 8.2%

Accuracy improved by +9.1%p

Filtered estimation consistently outperformed raw RSSI-based estimation across overall metrics.

2. Distance-Wise Metrics
Distance	Raw MAE	Filt MAE	Raw RMSE	Filt RMSE	Raw Acc (%)	Filt Acc (%)
0.1 m	0.014	0.013	0.018	0.013	99.7	100.0
0.4 m	0.071	0.069	0.076	0.072	94.3	97.7
0.7 m	0.082	0.078	0.094	0.084	91.7	89.7
1.0 m	0.234	0.225	0.288	0.235	1.7	3.0
1.3 m	0.842	0.811	0.924	0.854	1.3	1.7
1.6 m	0.152	0.085	0.182	0.110	0.0	64.3
2.0 m	0.589	0.585	0.620	0.604	4.3	0.7
3. Observations

Short-range regions (0.1 m–0.7 m) show high baseline accuracy.

Significant improvement observed at 1.6 m in accuracy (0.0% → 64.3%).

RMSE reduction indicates effective suppression of extreme RSSI spikes.

Mid-range distances (1.0 m–1.3 m) show persistent instability due to RSSI fluctuation characteristics.

4. Interpretation

The hybrid filtering strategy reduces large RSSI deviations and improves stability, particularly in regions sensitive to multipath interference.

The larger reduction in RMSE compared to MAE suggests that spike noise suppression was a primary contributor to performance improvement.

Repository Structure

firmware/ → Embedded RSSI processing code

analysis/ → Calibration & parameter estimation scripts

data/ → Experimental measurement data

docs/ → System architecture, experiment documentation, results

assets/images/ → Graphs, schematics, block diagrams

Key Contributions

Embedded multi-stage RSSI stabilization

Statistical outlier-based filtering

Adaptive weighted averaging

Distance-domain inertia compensation

Quantitative performance validation
