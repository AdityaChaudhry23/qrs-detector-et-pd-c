# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -Iinclude -O2

# Source and object files
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = build/qrs_detector

# Link everything
$(TARGET): $(OBJ) test/test_driver.c
	$(CC) $(CFLAGS) $^ -o $@

# Compile all .c into .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build
clean:
	rm -f src/*.o $(TARGET)

plot-raw-py:
	python3 plot/plot_raw_ecg.py

