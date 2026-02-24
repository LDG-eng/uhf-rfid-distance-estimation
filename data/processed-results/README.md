## processed-results/

Contains computed performance metrics derived from raw measurements.

Includes:

1. Overall performance summary  
   - MAE (raw vs filtered)  
   - RMSE (raw vs filtered)  
   - Accuracy within ±0.10 m  

2. Distance-wise performance metrics  
   - MAE per distance  
   - RMSE per distance  
   - Accuracy per distance  

Typical format:

distance_m, raw_mae, filtered_mae, raw_rmse, filtered_rmse, raw_accuracy, filtered_accuracy
