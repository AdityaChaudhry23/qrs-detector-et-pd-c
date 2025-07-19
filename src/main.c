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
    const char* derivative_path = "results/derivative_ecg.bin";
    const char* square_path = "results/squared_ecg.bin";

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

    // Allocate buffers for raw, filtered, derivative, and squared signals
    WFDB_Sample* v = malloc(sizeof(WFDB_Sample) * total_samples);
    double* raw_ecg = malloc(sizeof(double) * total_samples);
    double* filtered_ecg = malloc(sizeof(double) * total_samples);
    double* derivative_ecg = malloc(sizeof(double) * total_samples);
    double* squared_ecg = malloc(sizeof(double) * total_samples);

    if (!v || !raw_ecg || !filtered_ecg || !derivative_ecg || !squared_ecg) {
        fprintf(stderr, "Memory allocation failed\n");
        free(v); free(raw_ecg); free(filtered_ecg); free(derivative_ecg); free(squared_ecg);
        return 1;
    }

    // Read signal 0 (e.g., MLII) from the record
    int nsamp = 0;
    while (getvec(v + nsamp) > 0 && nsamp < total_samples) {
        raw_ecg[nsamp] = (double)v[nsamp];
        nsamp++;
    }

    printf("Read %d samples from record %s\n", nsamp, record_name);

    // Bandpass filter
    bandpass_filter(raw_ecg, filtered_ecg, nsamp, FILTER_LEN, FS);
    FILE* fout = fopen(result_path, "wb");
    if (!fout) {
        perror("fopen for filtered_ecg.bin");
        goto cleanup;
    }
    fwrite(filtered_ecg, sizeof(double), nsamp, fout);
    fclose(fout);
    printf("Filtered ECG written to %s\n", result_path);

    // Derivative filter
    if (derivative_filter(filtered_ecg, derivative_ecg, nsamp) != 0) {
        fprintf(stderr, "Derivative filter failed\n");
        goto cleanup;
    }
    FILE* fder = fopen(derivative_path, "wb");
    if (!fder) {
        perror("fopen for derivative_ecg.bin");
        goto cleanup;
    }
    fwrite(derivative_ecg, sizeof(double), nsamp, fder);
    fclose(fder);
    printf("Derivative ECG written to %s\n", derivative_path);

    // Squaring
    if (squaring_function(derivative_ecg, squared_ecg, nsamp) != 0) {
        fprintf(stderr, "Squaring function failed\n");
        goto cleanup;
    }
    FILE* fsq = fopen(square_path, "wb");
    if (!fsq) {
        perror("fopen for squared_ecg.bin");
        goto cleanup;
    }
    fwrite(squared_ecg, sizeof(double), nsamp, fsq);
    fclose(fsq);
    printf("Squared ECG written to %s\n", square_path);
    // Moving Window Integration
    double* integrated_ecg = malloc(sizeof(double) * total_samples);
    if (!integrated_ecg) {
        fprintf(stderr, "Memory allocation failed for integrated_ecg\n");
        goto cleanup;
    }

    size_t mwi_window = 30;  // ~80 ms at 360 Hz
    if (moving_window_integration(squared_ecg, integrated_ecg, nsamp, mwi_window) != 0) {
        fprintf(stderr, "Moving window integration failed\n");
        goto cleanup;
    }

    FILE* fmwi = fopen("results/integrated_ecg.bin", "wb");
    if (!fmwi) {
        perror("fopen for integrated_ecg.bin");
        goto cleanup;
    }
    fwrite(integrated_ecg, sizeof(double), nsamp, fmwi);
    fclose(fmwi);
    printf("Integrated ECG written to results/integrated_ecg.bin\n");
    cleanup:
        free(v);
        free(raw_ecg);
        free(filtered_ecg);
        free(derivative_ecg);
        free(squared_ecg);
        free(integrated_ecg);
        return 0;

}
