import numpy as np
import wfdb

# --- Config ---
RECORD_PATH   = "data/100"
ANNOTATOR     = "atr"
QRS_TXT       = "results/qrs_locs.txt"
TOLERANCE_MS  = 100  # Match within ±100 ms
FS            = 360
TOLERANCE_SAMPLES = int((TOLERANCE_MS / 1000.0) * FS)

# --- Load reference annotations ---
ann = wfdb.rdann(RECORD_PATH, extension=ANNOTATOR)
ref_peaks = ann.sample

# --- Load detected QRS peaks ---
detected_peaks = np.loadtxt(QRS_TXT, dtype=int)

# --- Compare with tolerance ---
tp = 0
fp = 0
fn = 0

matched = np.zeros_like(ref_peaks, dtype=bool)

for d in detected_peaks:
    if np.any(np.abs(ref_peaks - d) <= TOLERANCE_SAMPLES):
        closest_ref_idx = np.argmin(np.abs(ref_peaks - d))
        if not matched[closest_ref_idx]:
            tp += 1
            matched[closest_ref_idx] = True
        else:
            fp += 1  # Already matched, duplicate detection
    else:
        fp += 1  # No match

fn = np.sum(~matched)

# --- Compute metrics ---
sensitivity = tp / (tp + fn) if (tp + fn) else 0
ppv         = tp / (tp + fp) if (tp + fp) else 0

# --- Print results ---
print("Validation against 100.atr annotations")
print(f"Total Reference Peaks: {len(ref_peaks)}")
print(f"Detected Peaks:        {len(detected_peaks)}")
print(f"True Positives (TP):   {tp}")
print(f"False Positives (FP):  {fp}")
print(f"False Negatives (FN):  {fn}")
print(f"Sensitivity:           {sensitivity:.4f}")
print(f"PPV (Precision):       {ppv:.4f}")
