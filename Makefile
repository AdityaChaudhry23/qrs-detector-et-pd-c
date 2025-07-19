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

plot-raw-and-filtered:
	@echo "Plotting raw and filtered ECG signals..."
	@python3 plot/plot_raw_and_filtered.py


.PHONY: all clean run
