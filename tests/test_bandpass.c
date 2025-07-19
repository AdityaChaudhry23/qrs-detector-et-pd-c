#include <stdio.h>
#include <stdlib.h>
#include <wfdb/wfdb.h>
#include "../includes/qrs_detector.h"

#define MAX_SAMPLES 650000
#define FILTER_LEN 101
#define FS 360.0  // Sampling frequency for record 201

int main() {
    WFDB_Sample v[2];  // Assuming 2 channels: MLII and V1
    WFDB_Siginfo siginfo[2];

    // Change directory to test_data to locate the record
    if (isigopen("test_data/201", siginfo, 2) < 0) {
    fprintf(stderr, "Error: Cannot open WFDB record 201\n");
    return 1;
    }

    double* input = calloc(MAX_SAMPLES, sizeof(double));
    double* output = calloc(MAX_SAMPLES, sizeof(double));
    if (!input || !output) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    int sample_count = 0;
    while (getvec(v) >= 0 && sample_count < MAX_SAMPLES) {
        input[sample_count++] = (double)v[0];  // v[0] = MLII
    }

    printf("Loaded %d samples from record 201\n", sample_count);

    if (sample_count < FILTER_LEN) {
        fprintf(stderr, "Not enough samples for filter (need > %d)\n", FILTER_LEN);
        return 1;
    }

    // Apply bandpass filter
    printf("Applying bandpass filter...\n");
    bandpass_filter(input, output, sample_count, FILTER_LEN, FS);

    // Save to file
    FILE* f_out = fopen("test_results/test_bandpass.dat", "w");
    if (!f_out) {
        perror("Error opening output file");
        return 1;
    }

    for (int i = 0; i < sample_count; i++) {
        fprintf(f_out, "%d\t%lf\t%lf\n", i, input[i], output[i]);
    }

    fclose(f_out);
    printf("[PASS] Filtered ECG written to results/test_bandpass.dat\n");

    free(input);
    free(output);
    return 0;
}
