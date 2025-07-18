import pandas as pd
import matplotlib.pyplot as plt

# Load ECG and annotations
ecg = pd.read_csv('data/100.csv')
ann = pd.read_csv('data/100.ann.csv')

# Plot ECG signal
plt.figure(figsize=(12, 4))
plt.plot(ecg['MLII'], label='ECG (MLII)', color='blue')
plt.scatter(ann['sample'], ecg.loc[ann['sample'], 'MLII'], color='red', marker='x', label='R-peaks')
plt.title('Raw ECG with QRS Annotations')
plt.plot(ecg['V5'], label='ECG (V5)', color='green', alpha=0.5)
plt.xlabel('Sample Index')
plt.ylabel('Amplitude (mV)')
plt.grid(True)
plt.legend()
plt.tight_layout()
#plt.savefig('results/raw_ecg_py.png')
plt.show()
