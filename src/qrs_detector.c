#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
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

int derivative_filter(const double *input,
                      double       *output,
                      size_t        len)
{
    if (!input || !output || len == 0) return -1;          /* null‑guard  */
    if (len < 5) {                                         /* need 5 pts  */
        for (size_t i = 0; i < len; ++i) output[i] = 0.0;
        return 0;
    }

    /* leading zeros */
    for (size_t i = 0; i < 4; ++i) output[i] = 0.0;

    /* core loop – compiler will unroll & vectorise at ‑O3 */
    for (size_t n = 4; n < len; ++n) {
        output[n] = ( 2.0*input[n]
                    + 1.0*input[n-1]
                    - 1.0*input[n-3]
                    - 2.0*input[n-4]) * 0.125;             /* ÷8 = ×0.125 */
    }

    return 0;                                             /* success     */
}