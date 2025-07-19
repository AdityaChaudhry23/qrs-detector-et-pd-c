#include <stdlib.h>
#include "qrs_detector.h"  
#include <stdio.h>  

#define MAX_SAMPLES 650000

int main() {
    double ecg_raw[MAX_SAMPLES], ecg_filtered[MAX_SAMPLES];
    int sample_count = 0;

    // Load MLII signal from 100.csv
    FILE* f_in = fopen("data/203.csv", "r");
    if (!f_in) {
        perror("Failed to open ECG file");
        return 1;
    }

    char line[256];
    fgets(line, sizeof(line), f_in);  // Skip header

    while (fgets(line, sizeof(line), f_in) && sample_count < MAX_SAMPLES) {
        double ml2, v5;
        if (sscanf(line, "%lf,%lf", &ml2, &v5) == 2) {
            ecg_raw[sample_count++] = ml2;  // Use MLII channel
        }
    }
    fclose(f_in);

    // Apply bandpass filter
    bandpass_filter(ecg_raw, ecg_filtered, sample_count, 101, 360.0);

    // Write filtered output
    FILE* f_out = fopen("results/ecg_filtered.dat", "w");
    for (int i = 0; i < sample_count; i++) {
        fprintf(f_out, "%d\t%lf\t%lf\n", i, ecg_raw[i], ecg_filtered[i]);
    }
    fclose(f_out);

    printf("Filtered ECG saved to results/ecg_filtered.dat\n");
    return 0;
}
