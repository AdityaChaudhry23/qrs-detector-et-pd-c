#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define PI 3.14159265359

void bandpass_filter(double* input, double* output, int input_len, int filter_len, double fs) {
    double f1 = 5.0;   // Low cutoff
    double f2 = 15.0;  // High cutoff

    double* h_low_f2 = malloc(sizeof(double) * filter_len);
    double* h_low_f1 = malloc(sizeof(double) * filter_len);
    double* h_band = malloc(sizeof(double) * filter_len);

    if (!h_low_f2 || !h_low_f1 || !h_band) {
        fprintf(stderr, "Memory allocation failed\n");
        free(h_low_f2); free(h_low_f1); free(h_band);
        return;
    }

    int mid = filter_len / 2;

    // Low-pass for f2
    for (int i = 0; i < filter_len; i++) {
        int n = i - mid;
        h_low_f2[i] = (n == 0) ? 2 * f2 / fs : sin(2 * PI * f2 * n / fs) / (PI * n);
        h_low_f2[i] *= 0.54 - 0.46 * cos(2 * PI * i / (filter_len - 1)); // Hamming window
    }

    // Low-pass for f1 (for inversion to HPF)
    for (int i = 0; i < filter_len; i++) {
        int n = i - mid;
        h_low_f1[i] = (n == 0) ? 2 * f1 / fs : sin(2 * PI * f1 * n / fs) / (PI * n);
        h_low_f1[i] *= 0.54 - 0.46 * cos(2 * PI * i / (filter_len - 1));
        h_low_f1[i] = -h_low_f1[i]; // Spectral inversion
    }
    h_low_f1[mid] += 1.0;

    // Combine to create bandpass filter
    for (int i = 0; i < filter_len; i++) {
        h_band[i] = h_low_f2[i] + h_low_f1[i];
    }

    // Normalize
    double sum = 0.0;
    for (int i = 0; i < filter_len; i++) sum += h_band[i];
    if (sum != 0.0) {
        for (int i = 0; i < filter_len; i++) {
            h_band[i] /= sum;
        }
    }

    // Apply convolution (centered)
    for (int n = 0; n < input_len; n++) {
        output[n] = 0.0;
        for (int k = 0; k < filter_len; k++) {
            int idx = n - k + mid;
            if (idx >= 0 && idx < input_len) {
                output[n] += input[idx] * h_band[k];
            }
        }
    }

    // Zero-mean and normalize output
    double mean = 0.0;
    for (int i = 0; i < input_len; i++) mean += output[i];
    mean /= input_len;

    double max_val = 0.0;
    for (int i = 0; i < input_len; i++) {
        output[i] -= mean;
        if (fabs(output[i]) > max_val) max_val = fabs(output[i]);
    }

    if (max_val != 0.0) {
        for (int i = 0; i < input_len; i++) {
            output[i] /= max_val;
        }
    }

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

int squaring_function(const double *input, double *output, size_t len)
{
    if (!input || !output || len == 0) return -1;

    for (size_t i = 0; i < len; ++i) {
        output[i] = input[i] * input[i];
    }

    return 0;
}

int moving_window_integration(const double *input,
                               double       *output,
                               size_t        len,
                               size_t        window_size)
{
    if (!input || !output || len == 0 || window_size == 0) return -1;

    double sum = 0.0;

    // Pre-fill the sum for the first window
    for (size_t i = 0; i < window_size && i < len; ++i)
        sum += input[i];

    for (size_t i = 0; i < len; ++i) {
        if (i >= window_size) {
            sum -= input[i - window_size];
            sum += input[i];
        }
        output[i] = sum / window_size;
    }

    return 0;
}
