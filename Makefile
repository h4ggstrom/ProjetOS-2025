CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
LDFLAGS = -L/usr/local/lib -lcriterion
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(filter-out $(BUILD_DIR)/main.o, $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES)))
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_FILES))
TEST_BIN = $(BIN_DIR)/test_suite

all: $(BIN_DIR)/main

# Compilation de l'exécutable principal
$(BIN_DIR)/main: $(OBJ_FILES) $(BUILD_DIR)/main.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilation des fichiers objets pour les tests
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilation et exécution des tests
tests: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJ_FILES) $(filter-out $(BUILD_DIR)/main.o, $(OBJ_FILES))
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm -rf demo/*

.PHONY: all tests clean