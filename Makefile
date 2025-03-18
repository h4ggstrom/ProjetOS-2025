CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
LDFLAGS = -L/usr/local/lib -lcriterion
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)
TEST_BIN = $(BIN_DIR)/run_tests

all: $(BIN_DIR)/main

# Compilation de l'exécutable principal
$(BIN_DIR)/main: $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilation et exécution des tests
tests: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_FILES) $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all tests clean
