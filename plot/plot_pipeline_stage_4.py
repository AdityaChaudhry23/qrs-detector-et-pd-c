import wfdb
import numpy as np
import matplotlib.pyplot as plt

# --- Config ---
RECORD = '100'
RECORD_PATH = 'data/' + RECORD
MAX_SAMPLES = 4000

# --- Read raw ECG using WFDB ---
record = wfdb.rdrecord(RECORD_PATH, channels=[0])
raw_signal = record.p_signal[:MAX_SAMPLES, 0]

# --- Load processed signals ---
filtered = np.fromfile("results/filtered_ecg.bin", dtype=np.float64, count=MAX_SAMPLES)
derivative = np.fromfile("results/derivative_ecg.bin", dtype=np.float64, count=MAX_SAMPLES)
squared = np.fromfile("results/squared_ecg.bin", dtype=np.float64, count=MAX_SAMPLES)
integrated = np.fromfile("results/integrated_ecg.bin", dtype=np.float64, count=MAX_SAMPLES)

# --- Time axis ---
t = np.arange(MAX_SAMPLES)

# --- Plot all stages ---
plt.figure(figsize=(15, 12))

plt.subplot(5, 1, 1)
plt.plot(t, raw_signal, label='Raw ECG', color='blue')
plt.title("Raw ECG Signal")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(5, 1, 2)
plt.plot(t, filtered, label='Filtered ECG', color='green')
plt.title("Bandpass Filtered ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(5, 1, 3)
plt.plot(t, derivative, label='Derivative ECG', color='orange')
plt.title("Derivative ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(5, 1, 4)
plt.plot(t, squared, label='Squared ECG', color='purple')
plt.title("Squared ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(5, 1, 5)
plt.plot(t, integrated, label='Integrated ECG (MWI)', color='red')
plt.title("Integrated ECG (Moving Window)")
plt.xlabel("Samples")
plt.ylabel("Amplitude")
plt.grid(True)

plt.tight_layout()
plt.show()
