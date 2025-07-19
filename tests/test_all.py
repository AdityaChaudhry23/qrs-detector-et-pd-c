import numpy as np
import wfdb
import os
import pandas as pd

# --- Configuration ---
RECORDS = [
    "100", "101", "102", "103", "104", "105", "106", "107", "108", "109",
    "111", "112", "113", "114", "115", "116", "117", "118", "119", "121",
    "122", "123", "124", "200", "201", "202", "203", "205", "207", "208", "209",
    "210", "212", "213", "214", "215", "217", "219", "220", "221", "222", "223",
    "228", "230", "231", "232", "233", "234"
]
ANNOTATOR = "atr"
TOLERANCE_MS = 100
FS = 360
TOLERANCE_SAMPLES = int((TOLERANCE_MS / 1000.0) * FS)

# --- Results ---
results = []

for record in RECORDS:
    try:
        # Load reference annotations
        ann = wfdb.rdann(f"data/{record}", extension=ANNOTATOR)
        ref_peaks = ann.sample

        # Load detected QRS
        qrs_path = f"test_data/{record}_qrs_locs.txt"
        if not os.path.exists(qrs_path):
            print(f"[WARN] QRS detection file not found for {record}")
            continue

        detected_peaks = np.loadtxt(qrs_path, dtype=int)
        if detected_peaks.ndim == 0:
            detected_peaks = np.array([detected_peaks])

        # Compare peaks
        matched = np.zeros_like(ref_peaks, dtype=bool)
        tp = fp = 0

        for d in detected_peaks:
            close_indices = np.where(np.abs(ref_peaks - d) <= TOLERANCE_SAMPLES)[0]
            if close_indices.size > 0:
                closest_idx = close_indices[np.argmin(np.abs(ref_peaks[close_indices] - d))]
                if not matched[closest_idx]:
                    tp += 1
                    matched[closest_idx] = True
                else:
                    fp += 1
            else:
                fp += 1

        fn = np.sum(~matched)
        sensitivity = tp / (tp + fn) if (tp + fn) else 0
        precision = tp / (tp + fp) if (tp + fp) else 0
        f1 = 2 * (precision * sensitivity) / (precision + sensitivity) if (precision + sensitivity) else 0

        results.append({
            "Record": record,
            "Reference": len(ref_peaks),
            "Detected": len(detected_peaks),
            "TP": tp,
            "FP": fp,
            "FN": fn,
            "Sensitivity": round(sensitivity, 4),
            "PPV": round(precision, 4),
            "F1": round(f1, 4)
        })

    except Exception as e:
        print(f"[ERROR] Failed to process {record}: {e}")

# --- Print Table ---
print(f"{'Record':<6} {'Ref':>5} {'Det':>5} {'TP':>5} {'FP':>5} {'FN':>5} {'Sens':>8} {'PPV':>8} {'F1':>8}")
for r in results:
    print(f"{r['Record']:<6} {r['Reference']:5} {r['Detected']:5} {r['TP']:5} {r['FP']:5} {r['FN']:5} {r['Sensitivity']:8.4f} {r['PPV']:8.4f} {r['F1']:8.4f}")

# --- Compute and print averages ---
df = pd.DataFrame(results)
avg_sens = df["Sensitivity"].mean()
avg_ppv  = df["PPV"].mean()
avg_f1   = df["F1"].mean()

print("\nAverage Sensitivity: {:.4f}".format(avg_sens))
print("Average PPV (Precision): {:.4f}".format(avg_ppv))
print("Average F1 Score: {:.4f}".format(avg_f1))

# --- Save to CSV ---
os.makedirs("test_results", exist_ok=True)
df.to_csv("test_results/qrs_validation_summary.csv", index=False)
print("\nSaved validation summary to test_results/qrs_validation_summary.csv")
