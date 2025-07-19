import numpy as np
import matplotlib.pyplot as plt
import wfdb

# --- Constants ---
RECORD = "data/100"
MAX_SAMPLES = 4000
FILTERED_BIN = "results/filtered_ecg.bin"
DERIVATIVE_BIN = "results/derivative_ecg.bin"
SQUARED_BIN = "results/squared_ecg.bin"

# --- Load raw ECG using WFDB ---
record = wfdb.rdrecord(RECORD, sampto=MAX_SAMPLES, channels=[0])
raw_ecg = record.p_signal[:, 0]

# --- Load binary outputs from C ---
filtered_ecg = np.fromfile(FILTERED_BIN, dtype=np.float64, count=MAX_SAMPLES)
derivative_ecg = np.fromfile(DERIVATIVE_BIN, dtype=np.float64, count=MAX_SAMPLES)
squared_ecg = np.fromfile(SQUARED_BIN, dtype=np.float64, count=MAX_SAMPLES)

# --- Time axis ---
t = np.arange(MAX_SAMPLES)

# --- Plot pipeline stages ---
plt.figure(figsize=(14, 10))

plt.subplot(4, 1, 1)
plt.plot(t, raw_ecg, color='blue')
plt.title("Raw ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(4, 1, 2)
plt.plot(t, filtered_ecg, color='green')
plt.title("Bandpass Filtered ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(4, 1, 3)
plt.plot(t, derivative_ecg, color='orange')
plt.title("Derivative ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(4, 1, 4)
plt.plot(t, squared_ecg, color='red')
plt.title("Squared ECG")
plt.xlabel("Samples")
plt.ylabel("Amplitude")
plt.grid(True)

plt.tight_layout()
plt.show()
