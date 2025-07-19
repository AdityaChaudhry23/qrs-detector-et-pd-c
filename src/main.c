#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wfdb/wfdb.h>
#include "qrs_detector.h"

#define FILTER_LEN 201
#define FS 360.0  // Sampling frequency (Hz) for MIT-BIH

int main() {
    // Set path for WFDB record search
    setwfdb("data");
    char record_name[] = "100";
    const char* result_path = "results/filtered_ecg.bin";

    // Open record and load signal metadata
    WFDB_Siginfo si[2];
    if (isigopen(record_name, si, 2) < 1) {
        fprintf(stderr, "Could not open record %s\n", record_name);
        return 1;
    }

    // Determine sample count from header, fallback if needed
    const char* nsamp_str = getinfo("nsamp");
    int total_samples = 0;
    if (nsamp_str) {
        total_samples = strtol(nsamp_str, NULL, 10);
    }
    if (total_samples <= 0) {
        total_samples = 650000;  // fallback for MIT-BIH
    }

    // Allocate buffers for raw and filtered signal
    WFDB_Sample* v = malloc(sizeof(WFDB_Sample) * total_samples);
    double* raw_ecg = malloc(sizeof(double) * total_samples);
    double* filtered_ecg = malloc(sizeof(double) * total_samples);

    if (!v || !raw_ecg || !filtered_ecg) {
        fprintf(stderr, "Memory allocation failed\n");
        free(v); free(raw_ecg); free(filtered_ecg);
        return 1;
    }

    // Read signal 0 (e.g., MLII) from the record
    int nsamp = 0;
    while (getvec(v + nsamp) > 0 && nsamp < total_samples) {
        raw_ecg[nsamp] = (double)v[nsamp];  // signal 0
        nsamp++;
    }

    printf("Read %d samples from record %s\n", nsamp, record_name);

    // Apply bandpass filter (from qrs_detector.c)
    bandpass_filter(raw_ecg, filtered_ecg, nsamp, FILTER_LEN, FS);

    // Write output to binary file
    FILE* fout = fopen(result_path, "wb");
    if (!fout) {
        perror("fopen");
        free(v); free(raw_ecg); free(filtered_ecg);
        return 1;
    }

    fwrite(filtered_ecg, sizeof(double), nsamp, fout);
    fclose(fout);

    printf("Filtered ECG written to %s\n", result_path);

    // Cleanup
    free(v);
    free(raw_ecg);
    free(filtered_ecg);

    return 0;
}
