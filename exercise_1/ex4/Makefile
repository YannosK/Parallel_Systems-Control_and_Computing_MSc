# $(DEFINES) variable can be set to either one of the following or any combination of them:
# -DOUTPUT: Enable output mode
# -DDEFAULR: Enable default implementation of read-write lock (pthread_rwlock_t)
# -DREADER_PRIORITY_POLICY: Enable custom implementation of read-write lock that prioritizes readers
# -DWRITER_PRIORITY_POLICY: Enable custom implementation of read-write lock that prioritizes writers

CC = gcc
LIBS = -pthread
CFLAGS = -Wall -Wextra -p -pg -Iinclude $(DEFINES)

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