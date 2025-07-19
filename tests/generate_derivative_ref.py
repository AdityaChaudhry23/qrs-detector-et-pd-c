#!/usr/bin/env python3
import numpy as np

x = np.loadtxt("test_results/bandpass.dat")            # your existing dump
# derivative: y[n] = (2x[n] + x[n-1] - x[n-3] - 2x[n-4]) / 8
y = np.zeros_like(x)
if len(x) >= 5:
    y[4:] = (2*x[4:] + x[3:-1] - x[1:-3] - 2*x[0:-4]) / 8.0
np.savetxt("test_results/python_derivative.dat", y, fmt="%.18e")
print("✅  python_derivative.dat written")
