#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define PI 3.14159265359

void bandpass_filter(double* input, double* output, int input_len, int filter_len, double fs) {
    double f1 = 5.0;   // Low cutoff
    double f2 = 15.0;  // High cutoff

    // Allocate memory
    double* h_low_f2 = malloc(sizeof(double) * filter_len);
    double* h_low_f1 = malloc(sizeof(double) * filter_len);
    double* h_band = malloc(sizeof(double) * filter_len);

    if (!h_low_f2 || !h_low_f1 || !h_band) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // Design LPF with cutoff f2
    for (int i = 0; i < filter_len; i++) {
        int n = i - filter_len / 2;
        if (n == 0)
            h_low_f2[i] = 2 * f2 / fs;
        else
            h_low_f2[i] = sin(2 * PI * f2 * n / fs) / (PI * n);
        h_low_f2[i] *= 0.54 - 0.46 * cos(2 * PI * i / (filter_len - 1));  // Hamming window
    }

    // Design LPF with cutoff f1 (to later invert into HPF)
    for (int i = 0; i < filter_len; i++) {
        int n = i - filter_len / 2;
        if (n == 0)
            h_low_f1[i] = 2 * f1 / fs;
        else
            h_low_f1[i] = sin(2 * PI * f1 * n / fs) / (PI * n);
        h_low_f1[i] *= 0.54 - 0.46 * cos(2 * PI * i / (filter_len - 1));  // Hamming window
    }

    // Spectral inversion of LPF(f1) to get HPF(f1)
    for (int i = 0; i < filter_len; i++) {
        h_low_f1[i] = -h_low_f1[i];
    }
    h_low_f1[filter_len / 2] += 1;

    // Combine to make BPF: LPF(f2) + HPF(f1)
    for (int i = 0; i < filter_len; i++) {
        h_band[i] = h_low_f2[i] + h_low_f1[i];
    }

    // Normalize the filter (optional but helps consistency)
    double sum = 0.0;
    for (int i = 0; i < filter_len; i++) {
        sum += h_band[i];
    }
    if (sum != 0) {
        for (int i = 0; i < filter_len; i++) {
            h_band[i] /= sum;
        }
    }

    // Apply convolution
    for (int n = 0; n < input_len; n++) {
        output[n] = 0;
        for (int k = 0; k < filter_len; k++) {
            if (n - k >= 0) {
                output[n] += input[n - k] * h_band[k];
            }
        }
    }
    double mean = 0.0, max_val = 0.0;
    for (int i = 0; i < input_len; i++) {
        mean += output[i];
    }
    mean /= input_len;
    for (int i = 0; i < input_len; i++) {
        output[i] -= mean;
        if (fabs(output[i]) > max_val) {
            max_val = fabs(output[i]);
        }
    }
    if (max_val != 0.0) {
        for (int i = 0; i < input_len; i++) {
            output[i] /= max_val;
        }
    }
    // Clean up
    free(h_low_f2);
    free(h_low_f1);
    free(h_band);
}

void differentiate_signal(double* input, double* output, int len) {
    // Assumes fs = 360 Hz and uses the derivative coefficients from Pan-Tompkins paper
    // y(nT) = [2x(nT) + x(nT-T) - x(nT-3T) - 2x(nT-4T)] / 8T
    for (int i = 4; i < len; i++) {
        output[i] = (2 * input[i] + input[i - 1] - input[i - 3] - 2 * input[i - 4]) / 8.0;
    }

    // Set first few samples to 0 since they don't have enough history
    for (int i = 0; i < 4; i++) {
        output[i] = 0.0;
    }
}
