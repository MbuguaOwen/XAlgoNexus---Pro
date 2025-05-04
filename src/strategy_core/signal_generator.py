import logging
from datetime import datetime

from filters.kalman_spread_estimator import KalmanSpreadEstimator
from filters.ml_filter import MLFilter

# Configure logger
logger = logging.getLogger("signal_generator_v2")
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] %(levelname)s - %(name)s - %(message)s"
)

class SignalGenerator:
    def __init__(self, zscore_threshold=2.0):
        self.kalman = KalmanSpreadEstimator()
        self.ml_filter = MLFilter()
        self.zscore_threshold = zscore_threshold

    def generate_signal(self, features):
        if not features:
            logger.warning("[SIGNAL] Feature vector missing.")
            return None

        btc_price = features.get("btc_price")
        eth_price = features.get("eth_price")

        if btc_price is None or eth_price is None:
            logger.warning("[SIGNAL] Missing BTC or ETH price in feature vector.")
            return None

        # Update Kalman spread estimation
        spread = self.kalman.update(btc_price, eth_price)
        zscore = self.kalman.get_zscore()
        kalman_params = self.kalman.get_params()

        # Update feature vector with z-score for downstream use
        features["zscore"] = zscore

        # ML filter verdict (1 = approve, 0 = veto)
        verdict = self.ml_filter.predict(features)
        if verdict != 1:
            logger.info(f"[SIGNAL] ML vetoed | zscore={zscore:.4f}")
            return {
                "timestamp": features.get("timestamp", datetime.utcnow()),
                "decision": "HOLD",
                "side": None,
                "reason": "ML veto",
                "zscore": zscore
            }

        # Rule-based decision logic
        if zscore > self.zscore_threshold:
            decision = "SELL ETH, BUY BTC"
            side = "sell"
        elif zscore < -self.zscore_threshold:
            decision = "BUY ETH, SELL BTC"
            side = "buy"
        else:
            decision = "HOLD"
            side = None

        logger.info(
            f"[SIGNAL] Decision={decision} | zscore={zscore:.4f} | spread={spread:.6f} "
            f"| alpha={kalman_params['alpha']:.4f}, beta={kalman_params['beta']:.4f}"
        )

        return {
            "timestamp": features.get("timestamp", datetime.utcnow()),
            "decision": decision,
            "side": side,
            "zscore": zscore,
            "spread": spread,
            "kalman_alpha": kalman_params["alpha"],
            "kalman_beta": kalman_params["beta"],
            "btc_price": btc_price,
            "eth_price": eth_price,
            "volatility": features.get("volatility"),
            "imbalance": features.get("imbalance")
        }
