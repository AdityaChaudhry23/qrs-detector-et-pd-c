import numpy as np
import matplotlib.pyplot as plt
import wfdb

RAW_RECORD = "data/100"
FILTERED_BIN = "results/filtered_ecg.bin"
DERIVATIVE_BIN = "results/derivative_ecg.bin"
MAX_SAMPLES = 4000

# --- Load raw ECG using WFDB ---
record = wfdb.rdrecord(RAW_RECORD, channels=[0], sampto=MAX_SAMPLES)
raw_signal = record.p_signal[:, 0]

# --- Load filtered ECG ---
filtered_signal = np.fromfile(FILTERED_BIN, dtype=np.float64, count=MAX_SAMPLES)

# --- Load derivative ECG ---
derivative_signal = np.fromfile(DERIVATIVE_BIN, dtype=np.float64, count=MAX_SAMPLES)

# --- Time axis ---
t = np.arange(MAX_SAMPLES)

# --- Plot ---
plt.figure(figsize=(12, 9))

plt.subplot(3, 1, 1)
plt.plot(t, raw_signal, color='blue')
plt.title("Raw ECG (First 4000 Samples)")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(3, 1, 2)
plt.plot(t, filtered_signal, color='green')
plt.title("Bandpass Filtered ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(3, 1, 3)
plt.plot(t, derivative_signal, color='red')
plt.title("Derivative ECG")
plt.xlabel("Samples")
plt.ylabel("Amplitude")
plt.grid(True)

plt.tight_layout()
plt.show()
