import numpy as np
import joblib
import logging
from sklearn.ensemble import RandomForestClassifier
from prometheus_client import Counter

# Logger setup
logger = logging.getLogger("ml_filter")
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] %(levelname)s - %(name)s - %(message)s"
)

# Prometheus metrics
prediction_total = Counter(
    "mlfilter_total_predictions", "Total number of ML filter predictions"
)
prediction_correct = Counter(
    "mlfilter_correct_predictions", "Number of correct predictions by ML filter (rule-based heuristic)"
)

class MLFilter:
    def __init__(self, model_path="ml_model/random_forest_model.pkl"):
        self.model_path = model_path
        self.model = None
        self._load_model()

    def _load_model(self):
        try:
            self.model = joblib.load(self.model_path)
            logger.info(f"[MLFilter] Loaded model from {self.model_path}")
        except Exception as e:
            logger.warning(f"[MLFilter] Failed to load model from {self.model_path}: {e}")
            self.model = None

    def predict(self, fv: dict) -> int:
        if not self.model:
            logger.warning("[MLFilter] Model unavailable. Defaulting to allow signal.")
            return 1  # Fallback to 'allow' on model failure

        try:
            features = self.extract_features(fv)
            prediction = self.model.predict([features])[0]
            prediction_total.inc()

            # Rule-based approximation of correctness (used in Option 1)
            spread = fv.get("spread", 0.0)
            if (prediction == 1 and spread < 0) or (prediction == -1 and spread > 0):
                prediction_correct.inc()

            return int(prediction)

        except Exception as e:
            logger.error(f"[MLFilter] Prediction error: {e}")
            return 1  # Conservative fallback to avoid blocking signals

    def extract_features(self, fv: dict):
        try:
            return np.array([
                fv.get("spread", 0.0),
                fv.get("volatility", 0.0),
                fv.get("imbalance", 0.0),
                fv.get("zscore", 0.0),
            ])
        except Exception as e:
            logger.error(f"[MLFilter] Feature extraction error: {e}")
            return np.array([0.0, 0.0, 0.0, 0.0])
