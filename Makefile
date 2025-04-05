## @file
#  @brief Makefile du projet - Gestion de la compilation et des tests
#  @details Ce Makefile gère :
#  - La compilation du programme principal
#  - La compilation et l'exécution des tests unitaires
#  - Le nettoyage des fichiers générés

## @defgroup variables Variables du Makefile
#  @{

## @var CC
#  @brief Compilateur C à utiliser (gcc par défaut)
CC = gcc

## @var CFLAGS
#  @brief Options de compilation :
#  - -Iinclude : Chemin des headers
#  - -Wall -Wextra : Avertissements
#  - -g : Informations de débogage
CFLAGS = -Iinclude -Wall -Wextra -g

## @var LDFLAGS
#  @brief Options d'édition de liens :
#  - -L/usr/local/lib : Chemin des librairies
#  - -lcriterion : Librairie de tests unitaires
LDFLAGS = -L/usr/local/lib -lcriterion


## @var SRC_DIR
#  @brief Répertoire des sources (src par défaut)
SRC_DIR = src


## @var TEST_DIR
#  @brief Répertoire des tests (tests par défaut)
TEST_DIR = tests


## @var BUILD_DIR
#  @brief Répertoire des fichiers objets (build par défaut)
BUILD_DIR = build

## @var BIN_DIR
#  @brief Répertoire des exécutables (bin par défaut)
BIN_DIR = bin


## @var SRC_FILES
#  @brief Liste des fichiers sources (.c dans SRC_DIR)
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)


## @var OBJ_FILES
#  @brief Liste des fichiers objets (sans main.o)
OBJ_FILES = $(filter-out $(BUILD_DIR)/main.o, $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES)))

## @var TEST_FILES
#  @brief Liste des fichiers de tests (.c dans TEST_DIR)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

## @var TEST_OBJ_FILES
#  @brief Liste des fichiers objets des tests
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_FILES))


## @var TEST_BIN
#  @brief Exécutable des tests (bin/test_suite)
TEST_BIN = $(BIN_DIR)/test_suite

## @}

## @defgroup targets Cibles principales
#  @{

## @target all
#  @brief Cible par défaut - Construit l'exécutable principal
#  @details Dépend de $(BIN_DIR)/main
all: $(BIN_DIR)/main


## @target $(BIN_DIR)/main
#  @brief Compile l'exécutable principal
#  @param OBJ_FILES Fichiers objets nécessaires
#  @param BUILD_DIR/main.o Fichier objet principal
$(BIN_DIR)/main: $(OBJ_FILES) $(BUILD_DIR)/main.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

## @target $(BUILD_DIR)/%.o
#  @brief Compilation des fichiers objets
#  @param SRC_DIR/%.c Fichier source correspondant
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

## @target $(BUILD_DIR)/%.o (tests)
#  @brief Compilation des fichiers objets pour les tests
#  @param TEST_DIR/%.c Fichier de test correspondant
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

## @target tests
#  @brief Compile et exécute les tests unitaires
#  @details Dépend de $(TEST_BIN)
#  @sa $(TEST_BIN)
tests: $(TEST_BIN)
	./$(TEST_BIN)

## @target $(TEST_BIN)
#  @brief Compile la suite de tests
#  @param TEST_OBJ_FILES Fichiers objets des tests
#  @param OBJ_FILES Fichiers objets du programme (sans main.o)
$(TEST_BIN): $(TEST_OBJ_FILES) $(filter-out $(BUILD_DIR)/main.o, $(OBJ_FILES))
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

## @target clean
#  @brief Nettoie les fichiers générés
#  @details Supprime :
#  - Le répertoire build/
#  - Le répertoire bin/
#  - Les fichiers dans demo/
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm -rf demo/*

## @}

.PHONY: all tests clean