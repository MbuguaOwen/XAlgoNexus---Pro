# generate_dummy_model.py
import pickle
from sklearn.ensemble import RandomForestClassifier
import numpy as np

# Dummy dataset (X: features, y: labels)
X = np.random.rand(100, 5)
y = np.random.randint(0, 2, size=100)

# Train dummy model
model = RandomForestClassifier()
model.fit(X, y)

# Save the model
with open("ml_model/random_forest_model.pkl", "wb") as f:
    pickle.dump(model, f)

print("[✔] Dummy model saved to ml_model/random_forest_model.pkl")
