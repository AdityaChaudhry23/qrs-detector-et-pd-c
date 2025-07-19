import numpy as np
import matplotlib.pyplot as plt
import wfdb

RAW_RECORD = "data/100"
FILTERED_BIN = "results/filtered_ecg.bin"
MAX_SAMPLES = 4000

# --- Load raw ECG using WFDB ---
record = wfdb.rdrecord(RAW_RECORD, channels=[0], sampto=MAX_SAMPLES)
raw_signal = record.p_signal[:, 0]  # MLII, first channel

# --- Load filtered ECG ---
filtered_signal = np.fromfile(FILTERED_BIN, dtype=np.float64, count=MAX_SAMPLES)

# --- Time axis ---
t = np.arange(len(filtered_signal))

# --- Plot ---
plt.figure(figsize=(12, 6))

plt.subplot(2, 1, 1)
plt.plot(t, raw_signal, label='Raw ECG', color='blue')
plt.title("Raw ECG (First 4000 Samples)")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(2, 1, 2)
plt.plot(t, filtered_signal, label='Filtered ECG', color='green')
plt.title("Bandpass Filtered ECG (First 4000 Samples)")
plt.xlabel("Samples")
plt.ylabel("Amplitude")
plt.grid(True)

plt.tight_layout()
plt.show()
