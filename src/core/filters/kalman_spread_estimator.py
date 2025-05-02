import numpy as np

class KalmanSpreadEstimator:
    def __init__(self, initial_alpha=0.0, initial_beta=1.0, Q=1e-5, R=1e-3):
        self.alpha = initial_alpha  # Intercept
        self.beta = initial_beta    # Slope
        self.Q = Q  # Process noise covariance
        self.R = R  # Observation noise covariance

        self.P = np.eye(2)  # Covariance matrix
        self.state = np.array([self.alpha, self.beta])
        self.last_spread = 0
        self.spread_std = 1.0  # To compute z-score

        self.spread_history = []

    def update(self, x, y):
        """
        x = BTC price (independent)
        y = ETH price (dependent)
        """
        phi = np.array([1, x])  # Observation matrix
        y_pred = np.dot(phi, self.state)

        # Kalman gain
        P_phi = self.P @ phi
        S = phi @ P_phi + self.R
        K = P_phi / S

        # Residual (innovation)
        residual = y - y_pred

        # State update
        self.state += K * residual
        self.P = self.P - np.outer(K, phi) @ self.P + self.Q * np.eye(2)

        # Spread = actual - predicted
        spread = residual
        self.last_spread = spread
        self.spread_history.append(spread)

        # Update rolling std
        if len(self.spread_history) > 200:
            self.spread_history = self.spread_history[-200:]
        self.spread_std = np.std(self.spread_history) or 1.0

        return spread

    def get_zscore(self):
        return self.last_spread / self.spread_std

    def get_params(self):
        return {'alpha': self.state[0], 'beta': self.state[1]}