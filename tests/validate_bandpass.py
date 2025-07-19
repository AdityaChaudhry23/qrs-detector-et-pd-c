import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import mean_squared_error
from scipy.stats import pearsonr

# === Filter Parameters ===
FILTER_LEN = 101
DELAY = (FILTER_LEN - 1) // 2

# === Load Python-filtered ECG (Reference) ===
py_df = pd.read_csv("test_data/ecg_201_filtered.csv")
py_filtered = py_df["MLII"].values

# === Load C-filtered ECG ===
c_df = pd.read_csv("test_results/test_bandpass.dat", sep="\t", names=["index", "raw", "filtered"])
c_filtered = c_df["filtered"].values

# === Align: Compensate for filter delay ===
py_filtered = py_filtered[DELAY:]
c_filtered = c_filtered[DELAY:]

# === Trim to equal length ===
min_len = min(len(py_filtered), len(c_filtered))
py_filtered = py_filtered[:min_len]
c_filtered = c_filtered[:min_len]

# === Normalize ===
py_filtered /= np.max(np.abs(py_filtered))
c_filtered /= np.max(np.abs(c_filtered))

# === Compute error metrics ===
error = c_filtered - py_filtered
mse = mean_squared_error(py_filtered, c_filtered)
corr, _ = pearsonr(py_filtered, c_filtered)
snr = 10 * np.log10(np.sum(py_filtered**2) / np.sum(error**2))
psnr = 20 * np.log10(np.max(np.abs(py_filtered)) / np.sqrt(mse))

# === Thresholds ===
THRESHOLDS = {
    "MSE": 0.01,
    "CORR": 0.95,
    "SNR": 5,
    "PSNR": 20,
}

def result_line(metric, value):
    threshold = THRESHOLDS[metric]
    passed = (value <= threshold) if metric == "MSE" else (value >= threshold)
    status = "✅ PASS" if passed else "❌ FAIL"
    print(f"{metric:<10}: {value:.4f} (threshold: {threshold}) {status}")
    return passed

# === Print Results ===
print("\n📊 Bandpass Filter Validation Results:\n")
pass_flags = []
pass_flags.append(result_line("MSE", mse))
pass_flags.append(result_line("CORR", corr))
pass_flags.append(result_line("SNR", snr))
pass_flags.append(result_line("PSNR", psnr))

# === Final Verdict ===
if all(pass_flags):
    print("\n✅ OVERALL: PASS")
else:
    print("\n❌ OVERALL: FAIL")

# === Plot Comparison ===
plt.figure(figsize=(12, 4))
plt.plot(py_filtered[:4200], label="Python Filtered", alpha=0.75)
plt.plot(c_filtered[:4200], label="C Filtered", alpha=0.75)
plt.title("Bandpass Filter Output Comparison (First 4500 Samples)")
plt.xlabel("Sample Index")
plt.ylabel("Amplitude (normalized)")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
