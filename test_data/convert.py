import os
import wfdb
import pandas as pd
from scipy.signal import firwin, lfilter

# === Settings ===
record_name = "201"
raw_csv_path = "ecg_201_raw.csv"
filtered_csv_path = "ecg_201_filtered.csv"
filter_len = 101
fs_expected = 360
lowcut = 5
highcut = 15

# === 1. Load the ECG record ===
record = wfdb.rdrecord(record_name)  # Make sure .dat, .hea, etc. are in current dir
signal = record.p_signal
fs = record.fs
channels = record.sig_name

print(f"📥 Loaded record '{record_name}'")
print(f"→ Channels: {channels}")
print(f"→ Sampling rate: {fs} Hz")
print(f"→ Signal shape: {signal.shape}")

# === 2. Sanity check sampling rate ===
if fs != fs_expected:
    raise ValueError(f"Unexpected sampling rate {fs}. Expected {fs_expected} Hz.")

# === 3. Save raw signal to CSV ===
df_raw = pd.DataFrame(signal, columns=channels)
#os.makedirs(os.path.dirname(raw_csv_path), exist_ok=True)
df_raw.to_csv(raw_csv_path, index=False)
print(f"✅ Raw ECG saved to {raw_csv_path}")

# === 4. Apply FIR bandpass filter to MLII channel ===
mlii = df_raw["MLII"].values
taps = firwin(numtaps=filter_len, cutoff=[lowcut, highcut], fs=fs, pass_zero=False, window='hamming')
filtered = lfilter(taps, 1.0, mlii)

# === 5. Save filtered signal to CSV (include dummy V5 column) ===
df_filtered = pd.DataFrame({
    "MLII": filtered,
    "V5": [0.0] * len(filtered)
})
#os.makedirs(os.path.dirname(filtered_csv_path), exist_ok=True)
df_filtered.to_csv(filtered_csv_path, index=False)
print(f"✅ Filtered ECG saved to {filtered_csv_path}")
