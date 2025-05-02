
class MLFilter:
    def predict(self, fv: dict):
        # Placeholder logic – replace with real ML
        if fv.get("volatility", 0) > 5e-6:
            return 0  # block if too noisy
        return 1  # allow
