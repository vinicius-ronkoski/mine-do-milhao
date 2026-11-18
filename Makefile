# =============================================================================
#  Makefile - Mine do Milhão (FDCO2B - UTFPR Apucarana)
# =============================================================================

ifdef OS
  OS := $(strip $(OS))
else
  OS := $(strip $(shell uname))
endif

USE_RAYLIB ?= 0

BINNAME   = milhao
SRC_DIR   = src
INC_DIR   = include
BUILD_DIR = build
DATA_DIR = data

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP = $(OBJ:.o=.d)
DATA = $(wildcard $(DATA_DIR)/*.dat)

CC       = gcc
CFLAGS   = -Wall -Wextra -Werror -std=c99 -g -I./$(INC_DIR)
LIBDIR   =
LDLIBS   = -lm

ifeq ($(OS),Windows_NT)
	BIN       = $(BINNAME).exe
	RUN       = $(BIN)
	RM        = del /Q /F
	CLEAN_FILES = $(subst /,\,$(OBJ) $(DEP)) $(BIN)
	CLEAN_DATA = $(subst /,\,$(DATA))
	ifeq ($(USE_RAYLIB),1)
		LIBDIR = -L./libwin
		LDLIBS += -lraylib -lopengl32 -lgdi32 -lwinmm
	endif
else
	BIN       = $(BINNAME)
	RUN       = ./$(BIN)
	RM        = rm -f
	CLEAN_FILES = $(OBJ) $(DEP) $(BIN)
	CLEAN_DATA = $(DATA)
	ifeq ($(USE_RAYLIB),1)
		LIBDIR = -L./lib
		LDLIBS += -lraylib -lGL -lpthread -ldl -lrt -lX11
	endif
endif

ifeq ($(USE_RAYLIB),1)
	CFLAGS += -Wno-missing-braces
endif

.PHONY: all run debug valgrind clean help

all: $(BIN)

# Liga os objetos (as bibliotecas vão DEPOIS dos objetos).
$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBDIR) $(LDLIBS)

# Compila cada src/*.c em build/*.o (-MMD gera os .d de dependência dos .h).
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Executa a partir da raiz do projeto (os caminhos data/... dependem disso).
run: all
	$(RUN)

debug: all
	gdb $(BIN)

valgrind: all
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes $(RUN)

# Remove objetos, dependências e o executável (não mexe em data/).
clean:
	-$(RM) $(CLEAN_FILES)

# Limpa arquivos.dat em (data/)
clear_data:
	-$(RM) $(CLEAN_DATA)

help:
	@echo Alvos: all, run, clean, debug, valgrind, clear_data
	@echo Raylib: make USE_RAYLIB=1

-include $(DEP)
