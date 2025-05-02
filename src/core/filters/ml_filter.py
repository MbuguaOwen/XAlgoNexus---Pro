import numpy as np
from sklearn.ensemble import RandomForestClassifier
import joblib

class MLFilter:
    def __init__(self, model_path="ml_model/random_forest_model.pkl"):
        try:
            self.model = joblib.load(model_path)
        except Exception as e:
            print(f"[MLFilter] Failed to load model: {e}")
            self.model = None

    def predict(self, fv: dict) -> int:
        if not self.model:
            return 1  # Default to allow if model unavailable
        try:
            features = self.extract_features(fv)
            prediction = self.model.predict([features])[0]
            return int(prediction)
        except Exception as e:
            print(f"[MLFilter] Prediction error: {e}")
            return 1  # Fail-safe default

    def extract_features(self, fv: dict):
        # Example: Extract selected features for prediction
        return np.array([
            fv.get("spread", 0),
            fv.get("volatility", 0),
            fv.get("imbalance", 0),
            fv.get("zscore", 0),
        ])