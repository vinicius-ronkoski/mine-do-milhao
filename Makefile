# =============================================================================
#  Makefile - Mine do Milhão (FDCO2B - UTFPR Apucarana)
#
#  make                 compila só o modo texto (cadastro de perguntas)
#  make USE_RAYLIB=1    compila também o jogo visual (opção 8 do menu)
#  make run             compila e executa          make teste   testa a lógica sem janela
#  make clean           apaga objetos/executáveis  make clear_data  apaga data/*.dat
#
#  Funciona no Windows (MinGW, mingw32-make) e no Linux (gcc + make).
# =============================================================================

# -----------------------------------------------------------------------------
# Sistema operacional (no Windows a variável de ambiente OS vale Windows_NT)
# -----------------------------------------------------------------------------
ifdef OS
  OS := $(strip $(OS))
else
  OS := $(strip $(shell uname))
endif

USE_RAYLIB ?= 0

# Cada modo (e cada compilador) compila numa pasta própria: trocar de modo ou de gcc
# não mistura objetos. Ex.: build/raylib-x86_64-w64-mingw32/, build/texto-x86_64-linux-gnu/
ifeq ($(USE_RAYLIB),1)
  MODO = raylib
else
  MODO = texto
endif

# -----------------------------------------------------------------------------
# Pastas e arquivos
# -----------------------------------------------------------------------------
BINNAME    = milhao
SRC_DIR    = src
INC_DIR    = include
BUILD_ROOT = build
BUILD_DIR  = $(BUILD_ROOT)/$(MODO)-$(ALVO_GCC)
DATA_DIR   = data
RAYLIB_DIR = third_party/raylib

SRC  = $(wildcard $(SRC_DIR)/*.c)
OBJ  = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP  = $(OBJ:.o=.d)
DATA = $(wildcard $(DATA_DIR)/*.dat)

# Tudo que já foi compilado, nos dois modos (para o clean)
COMPILADOS = $(wildcard $(BUILD_ROOT)/*/*.o $(BUILD_ROOT)/*/*.d)

# Teste da lógica (só módulos sem raylib: funciona com ou sem USE_RAYLIB)
TESTE_SRC  = tests/teste_partida.c
TESTE_OBJS = $(addprefix $(BUILD_DIR)/,arquivo.o banco.o boss.o partida.o pergunta.o sorteio.o utils.o)

# -----------------------------------------------------------------------------
# Compilador
# -----------------------------------------------------------------------------
CC     = gcc

# Para qual sistema o gcc gera programas (ex.: x86_64-w64-mingw32, mingw32, x86_64-linux-gnu)
ALVO_GCC := $(shell $(CC) -dumpmachine)
CFLAGS = -Wall -Wextra -Werror -std=c99 -g -I./$(INC_DIR)
LIBDIR =

# Biblioteca OpenGL no Linux. Se o link falhar com "cannot find -lGL"
# (falta o pacote libgl-dev), use: make USE_RAYLIB=1 GL_LIB=-l:libGL.so.1
GL_LIB ?= -lGL

# -----------------------------------------------------------------------------
# Comandos e bibliotecas de cada sistema
#
# No Windows há dois casos:
#   - make rodando com o cmd (MinGW clássico, mingw32-make): usa del/mkdir com barra invertida;
#   - make rodando com sh (w64devkit, MSYS2, Git Bash): usa rm/mkdir -p como no Linux.
# O teste: no cmd "echo %OS%" imprime Windows_NT; no sh imprime o texto %OS% sem trocar.
# -----------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
	BIN           = $(BINNAME).exe
	TESTE_BIN     = teste_partida.exe
	RAYLIB_LIBS   = -lraylib -lopengl32 -lgdi32 -lwinmm
	RAYLIB_LIBDIR = -L./$(RAYLIB_DIR)/lib/windows
	ifeq ($(shell echo %OS%),Windows_NT)
		RUN         = $(BIN)
		TESTE_RUN   = $(TESTE_BIN)
		RM          = del /Q /F
		MKDIR_BUILD = mkdir $(subst /,\,$(BUILD_DIR))
		CLEAN_FILES = $(subst /,\,$(COMPILADOS)) $(BIN) $(TESTE_BIN)
		CLEAN_DATA  = $(subst /,\,$(DATA))
	else
		RUN         = ./$(BIN)
		TESTE_RUN   = ./$(TESTE_BIN)
		RM          = rm -f
		MKDIR_BUILD = mkdir -p $(BUILD_DIR)
		CLEAN_FILES = $(COMPILADOS) $(BIN) $(TESTE_BIN)
		CLEAN_DATA  = $(DATA)
	endif
else
	BIN           = $(BINNAME)
	RUN           = ./$(BIN)
	TESTE_BIN     = teste_partida
	TESTE_RUN     = ./$(TESTE_BIN)
	RM            = rm -f
	MKDIR_BUILD   = mkdir -p $(BUILD_DIR)
	CLEAN_FILES   = $(COMPILADOS) $(BIN) $(TESTE_BIN)
	CLEAN_DATA    = $(DATA)
	RAYLIB_LIBS   = -lraylib $(GL_LIB) -lpthread -ldl -lrt -lX11
	RAYLIB_LIBDIR = -L./$(RAYLIB_DIR)/lib/linux
endif

# -----------------------------------------------------------------------------
# A raylib de third_party/raylib/lib/windows é de 64 bits. Um gcc de 32 bits
# (ex.: MinGW clássico em C:\MinGW, "gcc -dumpmachine" = mingw32) compila tudo,
# mas o link falha com "undefined reference to InitWindow". Avisa antes de compilar.
# -----------------------------------------------------------------------------
ifeq ($(USE_RAYLIB),1)
ifeq ($(OS),Windows_NT)
ifeq ($(findstring x86_64,$(ALVO_GCC)),)
$(error O gcc instalado gera programas de 32 bits ($(ALVO_GCC)), mas a raylib do projeto e de 64 bits. Instale um gcc de 64 bits (MinGW-w64, ex.: w64devkit) e coloque-o no PATH antes do antigo. Passo a passo em docs/ARQUITETURA.md, secao 9. O modo texto (make sem USE_RAYLIB) continua funcionando com o gcc atual)
endif
endif
endif

# Com raylib: define USE_RAYLIB no código, acha o raylib.h e liga a biblioteca.
# A ordem importa na biblioteca estática: raylib ANTES de -lm (ela usa sqrtf, sinf...).
ifeq ($(USE_RAYLIB),1)
	CFLAGS += -Wno-missing-braces -DUSE_RAYLIB -I./$(RAYLIB_DIR)/include
	LIBDIR  = $(RAYLIB_LIBDIR)
	LDLIBS  = $(RAYLIB_LIBS) -lm
else
	LDLIBS  = -lm
endif

.PHONY: all run debug valgrind clean clear_data help teste FORCE

# -----------------------------------------------------------------------------
# Alvos principais
# -----------------------------------------------------------------------------
all: $(BIN)

# Liga os objetos (as bibliotecas vão DEPOIS dos objetos). O link roda sempre (FORCE):
# assim o executável nunca fica com o modo antigo depois de trocar USE_RAYLIB.
$(BIN): $(OBJ) FORCE
	$(CC) $(OBJ) -o $@ $(LIBDIR) $(LDLIBS)

FORCE:

# Compila cada src/*.c em build/*.o (-MMD gera os .d de dependência dos .h).
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# No Windows o mkdir do cmd cria as pastas intermediárias (build\raylib) com barra invertida.
$(BUILD_DIR):
	$(MKDIR_BUILD)

# Executa a partir da raiz do projeto (os caminhos data/... e assets/... dependem disso).
run: all
	$(RUN)

# -----------------------------------------------------------------------------
# Testes e depuração
# -----------------------------------------------------------------------------
teste: $(TESTE_OBJS) $(TESTE_SRC)
	$(CC) $(CFLAGS) $(TESTE_SRC) $(TESTE_OBJS) -o $(TESTE_BIN) -lm
	$(TESTE_RUN)

debug: all
	gdb $(BIN)

valgrind: all
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes $(RUN)

# -----------------------------------------------------------------------------
# Limpeza
# -----------------------------------------------------------------------------
# Remove objetos, dependências e executáveis dos dois modos (não mexe em data/).
clean:
	-$(RM) $(CLEAN_FILES)

# Apaga data/*.dat: na próxima execução o programa volta a ler o CSV.
clear_data:
	-$(RM) $(CLEAN_DATA)

help:
	@echo Alvos: all, run, clean, debug, valgrind, clear_data, teste
	@echo Raylib: make USE_RAYLIB=1

-include $(DEP)
