# ───────────────────────────────
#  Toolchain & global switches
# ───────────────────────────────
CC        ?= clang
DEBUG     ?= 0                    # use:  make DEBUG=1 all

CFLAGS    := -Wall -Wextra -Iincludes -Iexternal/wfdb/include
LDFLAGS   := -Lexternal/wfdb/lib -lwfdb -lm

ifeq ($(DEBUG),1)
  CFLAGS  += -O0 -g
else
  CFLAGS  += -O3 -march=native
endif

# ───────────────────────────────
#  Directory layout
# ───────────────────────────────
SRC_DIR          := src
BUILD_DIR        := build
TEST_DIR         := tests
RESULTS_DIR      := results
TEST_DATA_DIR    := test_data
TEST_RESULTS_DIR := test_results
PLOT_DIR         := plot

# ───────────────────────────────
#  Source → object lists
# ───────────────────────────────
SRC   := $(wildcard $(SRC_DIR)/*.c)
OBJ   := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

# ───────────────────────────────
#  Default target
# ───────────────────────────────
.PHONY: all
all: $(BUILD_DIR)/qrs_detector

# ───────────────────────────────
#  Build rules
# ───────────────────────────────
# 1. object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 2. final executable
$(BUILD_DIR)/qrs_detector: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# create build dir on‑the‑fly
$(BUILD_DIR):
	@mkdir -p $@

# ───────────────────────────────
#  Python helpers & validation
# ───────────────────────────────
.PHONY: generate-synthetic-data run-bandpass-test validate-bandpass
generate-synthetic-data:
	@echo "[PY] Generating synthetic ECG data…"
	python3 $(TEST_DATA_DIR)/generate_and_filter_ecg.py

run-bandpass-test: $(BUILD_DIR)
	@echo "[C] Running band‑pass filter test on synthetic ECG…"
	$(CC) $(TEST_DIR)/test_synthetic_bandpass.c \
	      $(SRC_DIR)/qrs_detector.c $(CFLAGS) $(LDFLAGS) \
	      -o $(BUILD_DIR)/test_synthetic_bandpass
	./$(BUILD_DIR)/test_synthetic_bandpass

validate-bandpass:
	python3 $(TEST_DIR)/validate_bandpass.py

# ───────────────────────────────
#  WFDB record 201/203 tests
# ───────────────────────────────
.PHONY: test-wfdb-bandpass
test-wfdb-bandpass: $(BUILD_DIR)
	$(CC) $(TEST_DIR)/test_bandpass.c $(SRC_DIR)/qrs_detector.c \
	      $(CFLAGS) $(LDFLAGS) -o $(BUILD_DIR)/test_bandpass
	LD_LIBRARY_PATH=external/wfdb/lib ./$(BUILD_DIR)/test_bandpass

# ───────────────────────────────
#  Plot helpers
# ───────────────────────────────
.PHONY: plot-raw-py
plot-raw-py:
	python3 $(PLOT_DIR)/plot_raw_ecg.py

# ───────────────────────────────
#  House‑keeping
# ───────────────────────────────
.PHONY: clean
clean:
	@echo "[CLEAN] Removing build, results and test artefacts…"
	@rm -rf $(BUILD_DIR) $(RESULTS_DIR)/*.dat $(TEST_RESULTS_DIR)/*.dat
