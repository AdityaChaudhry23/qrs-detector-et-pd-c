#ifndef QRS_DETECTOR_H
#define QRS_DETECTOR_H

// Bandpass filter for ECG (5–15 Hz)
// input: pointer to input ECG array
// output: pointer to output filtered array
// input_len: number of samples
// filter_len: number of filter taps (e.g., 101)
// fs: sampling frequency (e.g., 360.0)
void bandpass_filter(double* input,
                     double* output,
                     int input_len,
                     int filter_len,
                     double fs);

#endif  // QRS_DETECTOR_H
