BIN_DIR = bin
PROFILE_DIR = report

EXEC_NAME = main

all: dir run

dir:
	mkdir -p $(BIN_DIR)
	mkdir -p $(PROFILE_DIR)

run:
	nvcc -Iinclude -std=c++11 -o $(BIN_DIR)/$(EXEC_NAME) src/nbody.cu -run

profile: all
	nsys profile --stats=true -o $(PROFILE_DIR)/report ./$(BIN_DIR)/$(EXEC_NAME)

clean:
	rm -rf $(BIN_DIR) $(PROFILE_DIR)
