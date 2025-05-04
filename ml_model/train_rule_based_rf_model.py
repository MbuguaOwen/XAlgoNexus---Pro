import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, confusion_matrix
import joblib
import os

# 1. Load the feature set
df = pd.read_csv("features.csv")
print(f"[INFO] Loaded dataset with shape: {df.shape}")

# 2. Drop rows with missing values
df.dropna(inplace=True)

# 3. Create rule-based labels (Option 1 logic)
# Label: 1 = BUY signal (mean-reversion expected), -1 = SELL signal, 0 = HOLD
z = (df['spread'] - df['spread'].mean()) / df['spread'].std()
df['zscore'] = z

def label_signal(row):
    if row['zscore'] > 1.5:
        return -1  # mean-reversion SELL
    elif row['zscore'] < -1.5:
        return 1   # mean-reversion BUY
    else:
        return 0   # HOLD

df['signal_label'] = df.apply(label_signal, axis=1)

# 4. Features and target
features = df[['spread', 'volatility', 'imbalance', 'zscore']]
labels = df['signal_label']

# 5. Train/test split
X_train, X_test, y_train, y_test = train_test_split(features, labels, test_size=0.25, random_state=42, stratify=labels)

# 6. Train model
rf = RandomForestClassifier(n_estimators=100, max_depth=5, random_state=42, class_weight='balanced')
rf.fit(X_train, y_train)

# 7. Evaluate model
y_pred = rf.predict(X_test)
print("[INFO] Classification Report:\n", classification_report(y_test, y_pred))
print("[INFO] Confusion Matrix:\n", confusion_matrix(y_test, y_pred))

# 8. Save model
os.makedirs("ml_model", exist_ok=True)
joblib.dump(rf, "ml_model/random_forest_model.pkl")
print("[INFO] Model saved to ml_model/random_forest_model.pkl")
