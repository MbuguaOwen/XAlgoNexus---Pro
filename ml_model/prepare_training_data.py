# ml_model/prepare_training_data.py

import pandas as pd

# Load features
df = pd.read_csv("features.csv")

# Simple labeling heuristic (placeholder for real signal logic)
# Label as:
#  1 = Buy (e.g., positive edge),
# -1 = Sell (e.g., negative edge),
#  0 = Hold (e.g., no edge)
df["signal_label"] = df["spread"].apply(
    lambda x: 1 if x < -0.00001 else (-1 if x > 0.00001 else 0)
)

# Drop any NaNs or incomplete rows
df.dropna(inplace=True)

# Save labeled dataset
df.to_csv("labeled_training_data.csv", index=False)

print("✅ Labeled training data saved to labeled_training_data.csv")
