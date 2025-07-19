import wfdb
import pandas as pd
import numpy as np
from scipy.signal import lfilter

# === Load record 201 from MIT-BIH ===
record = wfdb.rdrecord("201")
signal = record.p_signal
fs = record.fs
channels = record.sig_name

print(f"Channels: {channels}")
print(f"Sampling rate: {fs} Hz")
print(f"Signal shape: {signal.shape}")

# === Extract MLII channel only ===
df_raw = pd.DataFrame(signal, columns=channels)
ml2 = df_raw["MLII"].values

# === Design Bandpass FIR Filter: 5–15 Hz ===
def bandpass_filter_c_style(input_signal, fs, f1=5.0, f2=15.0, filter_len=101):
    PI = np.pi
    M = filter_len
    n = np.arange(M)
    center = (M - 1) // 2

    def sinc_filter(fc):
        h = np.zeros(M)
        for i in range(M):
            k = i - center
            h[i] = 2 * fc / fs if k == 0 else np.sin(2 * PI * fc * k / fs) / (PI * k)
        h *= 0.54 - 0.46 * np.cos(2 * PI * n / (M - 1))
        return h

    h_low_f2 = sinc_filter(f2)
    h_low_f1 = sinc_filter(f1)
    h_high_f1 = -h_low_f1
    h_high_f1[center] += 1

    h_band = h_low_f2 + h_high_f1
    h_band /= np.sum(h_band)

    output = lfilter(h_band, [1.0], input_signal)
    return output

filtered = bandpass_filter_c_style(ml2, fs=fs)

df_filtered = pd.DataFrame({"MLII": filtered, "V5": np.zeros_like(filtered)})
df_filtered.to_csv("ecg_201_filtered.csv", index=False)
print("✅ Python filtered ECG saved using C-style FIR filter.")
