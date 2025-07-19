import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import mean_squared_error
from scipy.stats import pearsonr

# === Parameters ===
FILTER_LEN = 101
DELAY = (FILTER_LEN - 1) // 2

# === Load Python-filtered ECG (reference)
py_df = pd.read_csv("test_data/ecg_201_filtered.csv")
py_filtered = py_df["MLII"].values

# === Load C-filtered ECG
c_df = pd.read_csv("test_results/test_bandpass.dat", sep="\t", names=["index", "raw", "filtered"])
c_filtered = c_df["filtered"].values

# === Delay Compensation
py_filtered = py_filtered[DELAY:]
c_filtered = c_filtered[:len(py_filtered)]  # Match length

# === Normalize (optional but recommended)
py_filtered /= np.max(np.abs(py_filtered))
c_filtered /= np.max(np.abs(c_filtered))

# === Metrics ===
def snr(signal, noise):
    return 10 * np.log10(np.sum(signal**2) / np.sum(noise**2))

def psnr(signal, noise):
    peak = np.max(np.abs(signal))
    return 20 * np.log10(peak / np.sqrt(np.mean(noise**2)))

error = c_filtered - py_filtered
mse = mean_squared_error(py_filtered, c_filtered)
corr, _ = pearsonr(py_filtered, c_filtered)
snr_val = snr(py_filtered, error)
psnr_val = psnr(py_filtered, error)

# === Thresholds
THRESHOLDS = {
    "MSE": 0.01,
    "CORR": 0.95,
    "SNR": 5,
    "PSNR": 20,
}

# === Print Results
print("\n📊 Bandpass Filter Validation Results:\n")

def result_line(metric, value):
    threshold = THRESHOLDS[metric]
    status = "✅ PASS" if ((value >= threshold) if metric != "MSE" else (value <= threshold)) else "❌ FAIL"
    print(f"{metric:<10}: {value:.4f} (threshold: {threshold}) {status}")

result_line("MSE", mse)
result_line("CORR", corr)
result_line("SNR", snr_val)
result_line("PSNR", psnr_val)

# === Final Verdict
if mse <= THRESHOLDS["MSE"] and corr >= THRESHOLDS["CORR"] and snr_val >= THRESHOLDS["SNR"] and psnr_val >= THRESHOLDS["PSNR"]:
    print("\n✅ OVERALL: PASS")
else:
    print("\n❌ OVERALL: FAIL")

# === Plot Comparison
plt.figure(figsize=(12, 4))
plt.plot(py_filtered[:4000], label="Python Filtered", alpha=0.8)
plt.plot(c_filtered[:4000], label="C Filtered", alpha=0.8)
plt.title("Bandpass Filter Output Comparison")
plt.xlabel("Sample Index")
plt.ylabel("Amplitude")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
