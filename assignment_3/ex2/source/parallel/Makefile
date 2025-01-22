.PHONY: all clean

CFLAGS := -s -Wall -Wextra -O0
CC := mpicc

BUILD_DIR := build
EXECUTABLE := $(BUILD_DIR)/app
SRC_DIR := src
INC_DIR := inc
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))

all: $(BUILD_DIR) $(EXECUTABLE)
	@echo "Build finished"

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build dir"
