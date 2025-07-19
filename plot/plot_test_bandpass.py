import pandas as pd
import matplotlib.pyplot as plt

# Load ECG data
df = pd.read_csv("results/test_bandpass.dat", sep='\t', names=["index", "raw", "filtered"])

# ✅ Zoom into first 4000 samples
df = df[df["index"] < 4000]

# Plot with subplots
fig, axs = plt.subplots(2, 1, figsize=(14, 6), sharex=True)

axs[0].plot(df["index"], df["raw"], label="Raw ECG", color="blue", alpha=0.6)
axs[0].set_title("Raw ECG (MLII)")
axs[0].set_ylabel("Amplitude (mV)")
axs[0].grid(True)
axs[0].legend()

axs[1].plot(df["index"], df["filtered"], label="Filtered ECG (5–15 Hz)", color="red", alpha=0.9)
axs[1].set_title("Bandpass Filtered ECG")
axs[1].set_xlabel("Sample Index")
axs[1].set_ylabel("Amplitude (mV)")
axs[1].grid(True)
axs[1].legend()

plt.tight_layout()
plt.show()
