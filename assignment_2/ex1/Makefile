CC = gcc
CFLAGS = -Wall -Wextra -p -pg -Iinclude
LIBS = -lm -fopenmp

ifdef DEBUG
	CFLAGS += -g -O0 -DDEBUG
else
	CFLAGS += -O2
endif

# Save binaries and object files in the bin/ directory
BIN_DIR = bin
EXEC = $(BIN_DIR)/main

SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,$(BIN_DIR)/%.o,$(SRC))

all: $(BIN_DIR) $(EXEC)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Rule to build the executable
$(EXEC): $(OBJ)
	@$(CC) $(OBJ) -o $(EXEC) $(LIBS)

# Rule to compile .c files into .o files inside bin/ directory
$(BIN_DIR)/%.o: %.c
	@mkdir -p $(dir $@)  # Create necessary directories
	@$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

# Clean up generated files
clean:
	@rm -rf $(BIN_DIR)/*
