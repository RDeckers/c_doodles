rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *, %,$2),$d))
ifeq ($(OS),Windows_NT)
	CHK_DIR_EXISTS = if not exist "$(strip $1)" mkdir "$(strip $1)"
	NUKE = rmdir /s /q
	COPY_DIR = xcopy $1 $2 /E /H /Y
	COPY = xcopy $1 $2
	FIX_PATH = $(subst /,\,$1)
	LIB_SUFFIX = .dll
else
	CHK_DIR_EXISTS = test -d $1 || mkdir -p $1
	NUKE = rm -r $1
	COPY_DIR = cp -rv $1 $2
	COPY = cp $1 $2
	FIX_PATH = $1
	LIB_SUFFIX = .so
endif
PROJECT_DIR :=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OBJ_DIR := $(PROJECT_DIR)obj
BIN_DIR := $(PROJECT_DIR)bin
SRC_DIR := $(PROJECT_DIR)src
EXAMPLE_DIR := $(PROJECT_DIR)examples
LIB_DIR := $(BIN_DIR)
LIB_NAME := linalg

C_FILES := $(call rwildcard, $(SRC_DIR), *.c)
EXAMPLE_C_FILES := $(call rwildcard, $(EXAMPLE_DIR), *.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))
EXAMPLES := $(patsubst $(EXAMPLE_DIR)/%.c,$(EXAMPLE_BIN_DIR)/%,$(EXAMPLE_C_FILES))

C_FLAGS += --std=gnu99 -O3 -pipe -march=native -I$(PROJECT_DIR)headers

lib: $(C_FILES)
	$(call CHK_DIR_EXISTS, $(LIB_DIR))
	gcc -fpic -shared $(C_FLAGS) -o $(LIB_DIR)/lib$(LIB_NAME)$(LIB_SUFFIX) $<

all : 4x4 square verify verify_transpose

square : $(EXAMPLE_DIR)/MxM_square.c lib
	gcc $(C_FLAGS) -L $(LIB_DIR) -Wl,-R -Wl,$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) -Wl,-llinalg -o $(BIN_DIR)/MxM_square $(EXAMPLE_DIR)/MxM_square.c

4x4 : $(EXAMPLE_DIR)/MxM_4x4.c lib
	gcc $(C_FLAGS) -L $(LIB_DIR) -Wl,-R -Wl,$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) -Wl,-llinalg -o $(BIN_DIR)/MxM_4x4 $(EXAMPLE_DIR)/MxM_4x4.c

verify : $(EXAMPLE_DIR)/verify.c lib
	gcc $(C_FLAGS) -L $(LIB_DIR) -Wl,-R -Wl,$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) -Wl,-llinalg -o $(BIN_DIR)/verify $(EXAMPLE_DIR)/verify.c

verify_transpose : $(EXAMPLE_DIR)/verify_transpose.c lib
	gcc $(C_FLAGS) -L $(LIB_DIR) -Wl,-R -Wl,$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) -Wl,-llinalg -o $(BIN_DIR)/verify_transpose $(EXAMPLE_DIR)/verify_transpose.c

clean:
	$(NUKE) "$(BIN_DIR)"
