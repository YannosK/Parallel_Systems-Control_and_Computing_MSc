.PHONY: all clean

EXECUTABLE := calc_pi_seq

CFLAGS := -Wall -Wextra -O0 -s
CC := gcc

BUILD_DIR := build
SRC_DIR := src
INC_DIR := inc
SRC := $(SRC_DIR)/main.c $(SRC_DIR)/my_rand.c
OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

all: $(EXECUTABLE)
	@echo "Build finished"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build dir"

