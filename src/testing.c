#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wfdb/wfdb.h>
#include "qrs_detector.h"

#define MAX_SAMPLES 650000
#define MAX_FILENAME 256

int main() {
    const char* records[] = {
    "100", "101", "102", "103", "104", "105", "106", "107", "108", "109",
    "111", "112", "113", "114", "115", "116", "117", "118", "119", "121",
    "122", "123", "124", "200", "201", "202", "203", "205", "207", "208", "209",
    "210", "212", "213", "214", "215", "217", "219", "220", "221", "222", "223",
    "228", "230", "231", "232", "233", "234"
};  // Add more record names here
    const size_t num_records = sizeof(records) / sizeof(records[0]);

    for (size_t r = 0; r < num_records; ++r) {
        const char* record = records[r];

        // --- Load raw signal ---
        setwfdb("data");
        WFDB_Siginfo si[2];
        if (isigopen(record, si, 2) < 1) {
            fprintf(stderr, "Error opening record %s\n", record);
            continue;
        }

        WFDB_Sample* v = malloc(sizeof(WFDB_Sample) * MAX_SAMPLES);
        double* raw_ecg = malloc(sizeof(double) * MAX_SAMPLES);
        double* filtered_ecg = malloc(sizeof(double) * MAX_SAMPLES);
        double* derivative_ecg = malloc(sizeof(double) * MAX_SAMPLES);
        double* squared_ecg = malloc(sizeof(double) * MAX_SAMPLES);
        double* integrated_ecg = malloc(sizeof(double) * MAX_SAMPLES);
        int* qrs_locs = malloc(sizeof(int) * MAX_SAMPLES);

        if (!v || !raw_ecg || !filtered_ecg || !derivative_ecg || !squared_ecg || !integrated_ecg || !qrs_locs) {
            fprintf(stderr, "Memory allocation failed\n");
            continue;
        }

        int nsamp = 0;
        while (getvec(v + nsamp) > 0 && nsamp < MAX_SAMPLES) {
            raw_ecg[nsamp] = (double)v[nsamp];
            nsamp++;
        }

        // --- Preprocess ---
        bandpass_filter(raw_ecg, filtered_ecg, nsamp, 201, 360.0);
        derivative_filter(filtered_ecg, derivative_ecg, nsamp);
        squaring_function(derivative_ecg, squared_ecg, nsamp);
        moving_window_integration(squared_ecg, integrated_ecg, nsamp, 30);

        // --- Detect QRS peaks ---
        size_t num_peaks = 0;
        if (detect_qrs_peaks(integrated_ecg, qrs_locs, nsamp, &num_peaks) != 0) {
            fprintf(stderr, "QRS detection failed for %s\n", record);
            continue;
        }

        // --- Save results ---
        char filename[MAX_FILENAME];
        snprintf(filename, sizeof(filename), "test_data/%s_qrs_locs.txt", record);
        FILE* fout = fopen(filename, "w");
        if (!fout) {
            perror("fopen");
            continue;
        }

        for (size_t i = 0; i < num_peaks; ++i) {
            fprintf(fout, "%d\n", qrs_locs[i]);
        }
        fclose(fout);

        printf("Processed record %s — Detected %zu QRS peaks\n", record, num_peaks);

        // --- Free memory ---
        free(v);
        free(raw_ecg);
        free(filtered_ecg);
        free(derivative_ecg);
        free(squared_ecg);
        free(integrated_ecg);
        free(qrs_locs);
    }

    return 0;
}
