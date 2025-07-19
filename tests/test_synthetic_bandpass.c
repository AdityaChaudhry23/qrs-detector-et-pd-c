#include <stdio.h>
#include <stdlib.h>
#include "../includes/qrs_detector.h"

#define MAX_SAMPLES 650000
#define FILTER_LEN 101
#define FS 360.0

int main() {
    // === Allocate memory for input and output ===
    double* input = calloc(MAX_SAMPLES, sizeof(double));
    double* output = calloc(MAX_SAMPLES, sizeof(double));
    if (!input || !output) {
        fprintf(stderr, "❌ Memory allocation failed\n");
        return 1;
    }

    // === Load synthetic ECG from test_data/synthetic_input.csv ===
    int sample_count = 0;
    FILE* f_in = fopen("test_data/ecg_201_raw.csv", "r");
    if (!f_in) {
        perror("❌ Failed to open test_data/ecg_201_raw.csv");
        free(input);
        free(output);
        return 1;
    }

    char line[256];
    fgets(line, sizeof(line), f_in);  // Skip header

    while (fgets(line, sizeof(line), f_in) && sample_count < MAX_SAMPLES) {
        double ml2, v5;
        if (sscanf(line, "%lf,%lf", &ml2, &v5) == 2) {
            input[sample_count++] = ml2;  // Use MLII only
        }
    }
    fclose(f_in);

    printf("✅ Loaded %d samples from test_data/ecg_201_raw.csv\n", sample_count);

    if (sample_count < FILTER_LEN) {
        fprintf(stderr, "❌ Not enough samples to apply filter. Need ≥ %d\n", FILTER_LEN);
        free(input);
        free(output);
        return 1;
    }

    // === Apply bandpass filter ===
    printf("⏳ Applying bandpass filter...\n");
    bandpass_filter(input, output, sample_count, FILTER_LEN, FS);

    // === Save to test_results/test_bandpass.dat ===
    FILE* f_out = fopen("test_results/test_bandpass.dat", "w");
    if (!f_out) {
        perror("❌ Failed to open test_results/test_bandpass.dat");
        free(input);
        free(output);
        return 1;
    }

    for (int i = 0; i < sample_count; i++) {
        fprintf(f_out, "%d\t%lf\t%lf\n", i, input[i], output[i]);
    }
    fclose(f_out);

    printf("✅ [PASS] Bandpass output written to test_results/test_bandpass.dat\n");

    // === Cleanup ===
    free(input);
    free(output);
    return 0;
}
