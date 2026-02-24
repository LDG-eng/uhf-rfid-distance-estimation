## calibration/

RSSI measurements used to estimate log-distance model parameters.

Includes:

- RSSI samples collected at reference distance (1.0 m)
- RSSI samples collected at secondary distance (e.g., 3.0 m)

These data were used to compute:

- P0 (reference power)
- n (path-loss exponent)

Format:

distance_m, sample_index, rssi_dbm
