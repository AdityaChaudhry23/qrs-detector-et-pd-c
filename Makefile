# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -Iincludes -O0 -g
LDFLAGS = -lm

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
RESULTS_DIR = results
PLOT_DIR = plot
TEST_DATA_DIR = test_data
TEST_RESULTS_DIR = test_results

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)

# Default target
all: $(BUILD_DIR)/qrs_detector

# Build main detector (if needed)
$(BUILD_DIR)/qrs_detector: $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# 1. Generate synthetic ECG test data (Python)
generate-synthetic-data:
	@echo "[PY] Generating synthetic ECG data..."
	python3 $(TEST_DATA_DIR)/generate_and_filter_ecg.py

# 2. Run C-based bandpass filter on synthetic input
run-bandpass-test:
	@echo "[C] Running bandpass filter test on synthetic ECG..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_DIR)/test_synthetic_bandpass.c $(SRC_DIR)/qrs_detector.c -Iincludes -o $(BUILD_DIR)/test_synthetic_bandpass $(LDFLAGS)
	./$(BUILD_DIR)/test_synthetic_bandpass

# Compare C vs Python filtered ECG
validate-bandpass:
	python3 test_results/validate_bandpass.py
 

# Plot raw (MIT-BIH) ECG data
plot-raw-py:
	python3 $(PLOT_DIR)/plot_raw_ecg.py

# Clean build and results
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/* $(RESULTS_DIR)/*.dat
