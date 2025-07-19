# Compiler and Flags
CC = clang
CFLAGS = -Wall -O2 -Iincludes -Iexternal/wfdb/include
LDFLAGS = -Lexternal/wfdb/lib -lwfdb -lm

# Directories
SRC_DIR = src
OBJ_DIR = build
BIN = $(OBJ_DIR)/qrs_detector

# Source and Object files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/qrs_detector.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default target
all: $(BIN)

# Link final binary
$(BIN): $(OBJS)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN)
	@echo "Cleaned build artifacts."

# Run the binary
run: $(BIN)
	LD_LIBRARY_PATH=external/wfdb/lib ./$(BIN)

plot-stage-1:
	@echo "Plotting raw and filtered ECG signals..."
	@python3 plot/plot_pipeline_stage_1.py

plot-stage-2:
	@echo "Plotting raw, filtered, and derivative ECG signals..."
	@python3 plot/plot_pipeline_stage_2.py

plot-stage-3:
	@echo "Plotting raw, filtered, and derivative ECG signals..."
	@python3 plot/plot_pipeline_stage_3.py

plot-stage-4:
	@echo "Plotting raw, filtered,derivative and integrated ECG signals..."
	@python3 plot/plot_pipeline_stage_4.py

plot-stage-5:
	@python3 plot/plot_pipeline_stage_5.py

test-100:
	@python3 tests/test_100.py



test-all: build/qrs_detector.o
	$(CC) src/testing.c build/qrs_detector.o -o build/tester -Iincludes -Lexternal/wfdb/include -Lexternal/wfdb/lib -lwfdb -lm
	LD_LIBRARY_PATH=external/wfdb/lib ./build/tester
	@python3 tests/test_all.py


.PHONY: all clean run
