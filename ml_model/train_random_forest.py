# ml_model/train_random_forest.py

import pandas as pd
import numpy as np
import joblib
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report

# Load labeled dataset
df = pd.read_csv("labeled_training_data.csv")

# Features and labels
X = df[["spread", "volatility", "imbalance"]]
y = df["signal_label"]

# Train/test split
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Train model
model = RandomForestClassifier(n_estimators=100, random_state=42)
model.fit(X_train, y_train)

# Evaluation
y_pred = model.predict(X_test)
print("✅ Model Performance:\n")
print(classification_report(y_test, y_pred))

# Save the model
joblib.dump(model, "ml_model/random_forest_model.pkl")
print("✅ Trained model saved to ml_model/random_forest_model.pkl")
