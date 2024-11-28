.PHONY: all clean

CFLAGS := -Wall -Wextra -O0 -s
LDFLAGS := -lpthread
CC := gcc

BUILD_DIR := build
EXECUTABLE := $(BUILD_DIR)/app
SRC_DIR := src
INC_DIR := inc
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

all: $(EXECUTABLE)
	@echo "Build finished"

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build dir"


