rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *, %,$2),$d))
ifeq ($(OS),Windows_NT)
	CHK_DIR_EXISTS = if not exist "$(strip $1)" mkdir "$(strip $1)"
	NUKE = rmdir /s /q
	COPY_DIR = xcopy $1 $2 /E /H /Y
	FIX_PATH = $(subst /,\,$1)
else
	CHK_DIR_EXISTS = test -d $1 || mkdir -p $1
	NUKE = rm -r $1
	COPY_DIR = cp -rv $1 $2
	FIX_PATH = $1
endif
PROJECT_DIR :=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OBJ_DIR := $(PROJECT_DIR)obj
BIN_DIR := $(PROJECT_DIR)bin
SRC_DIR := $(PROJECT_DIR)src
EXAMPLE_DIR := $(PROJECT_DIR)examples
LIB_DIR := $(PROJECT_DIR)lib

C_FILES := $(call rwildcard, $(SRC_DIR), *.c)
EXAMPLE_C_FILES := $(call rwildcard, $(EXAMPLE_DIR), *.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))
EXAMPLES := $(patsubst $(EXAMPLE_DIR)/%.c,$(EXAMPLE_BIN_DIR)/%,$(EXAMPLE_C_FILES))

LD_FLAGS += --std=c99 -lpthread
C_FLAGS += --std=c99 -O2 -pipe -march=native -I$(PROJECT_DIR)headers

lib: $(OBJ_FILES)
	$(call CHK_DIR_EXISTS, $(LIB_DIR))
	gcc -shared $(C_FLAGS) -o $(LIB_DIR)/libmultiX.so $<

all : lib
	$(call CHK_DIR_EXISTS, $(BIN_DIR))
	$(foreach example, $(EXAMPLE_C_FILES), gcc $(C_FLAGS) $(LD_FLAGS) -L $(LIB_DIR) -Wl,-R -Wl,$(LIB_DIR) -lmultiX -o $(patsubst $(EXAMPLE_DIR)/%.c,$(BIN_DIR)/%,$(example)) $(example))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(call CHK_DIR_EXISTS, $(dir $@))
	@echo compiling $<
	gcc -fpic $(C_FLAGS) -o "$@" -c "$<"

clean:
	$(NUKE) "$(OBJ_DIR)" "$(BIN_DIR)" "$(LIB_DIR)"
