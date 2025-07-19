import numpy as np
import matplotlib.pyplot as plt
import wfdb

# --- Paths ---
RECORD_PATH     = "data/100"
FILTERED_BIN    = "results/filtered_ecg.bin"
DERIVATIVE_BIN  = "results/derivative_ecg.bin"
SQUARED_BIN     = "results/squared_ecg.bin"
INTEGRATED_BIN  = "results/integrated_ecg.bin"
QRS_PEAKS_TXT   = "results/qrs_locs.txt"
MAX_SAMPLES     = 4000

# --- Load raw ECG using WFDB ---
record = wfdb.rdrecord(RECORD_PATH, sampto=MAX_SAMPLES)
raw_ecg = record.p_signal[:, 0]  # Assuming MLII is channel 0

# --- Load processed signals ---
filtered_ecg    = np.fromfile(FILTERED_BIN, dtype=np.float64, count=MAX_SAMPLES)
derivative_ecg  = np.fromfile(DERIVATIVE_BIN, dtype=np.float64, count=MAX_SAMPLES)
squared_ecg     = np.fromfile(SQUARED_BIN, dtype=np.float64, count=MAX_SAMPLES)
integrated_ecg  = np.fromfile(INTEGRATED_BIN, dtype=np.float64, count=MAX_SAMPLES)
qrs_peaks       = np.loadtxt(QRS_PEAKS_TXT, dtype=int)
qrs_peaks       = qrs_peaks[qrs_peaks < MAX_SAMPLES]  # Filter out-of-bounds indices

# --- Refine QRS peaks to actual R-peak positions in filtered ECG ---
def refine_r_peaks(signal, peak_indices, window=15):
    refined = []
    for idx in peak_indices:
        start = max(0, idx - window)
        end = min(len(signal), idx + window)
        local_max = np.argmax(signal[start:end])
        refined.append(start + local_max)
    return np.array(refined)

r_peaks = refine_r_peaks(filtered_ecg, qrs_peaks, window=15)

# --- Time axis ---
t = np.arange(MAX_SAMPLES)

# --- Plot ---
plt.figure(figsize=(14, 12))

plt.subplot(6, 1, 1)
plt.plot(t, raw_ecg, color='blue')
plt.title("Raw ECG Signal")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(6, 1, 2)
plt.plot(t, filtered_ecg, color='green')
plt.title("Bandpass Filtered ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(6, 1, 3)
plt.plot(t, derivative_ecg, color='orange')
plt.title("Derivative ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(6, 1, 4)
plt.plot(t, squared_ecg, color='purple')
plt.title("Squared ECG")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(6, 1, 5)
plt.plot(t, integrated_ecg, color='red')
plt.scatter(qrs_peaks, integrated_ecg[qrs_peaks], color='black', label="QRS Peaks", zorder=5)
plt.title("Integrated ECG with QRS Peaks")
plt.ylabel("Amplitude")
plt.legend()
plt.grid(True)

plt.subplot(6, 1, 6)
plt.plot(t, filtered_ecg, color='green', label='Filtered ECG')
plt.scatter(r_peaks, filtered_ecg[r_peaks], color='black', label="Refined QRS Peaks", zorder=5)
plt.title("Filtered ECG with Refined QRS Peaks")
plt.xlabel("Samples")
plt.ylabel("Amplitude")
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()
