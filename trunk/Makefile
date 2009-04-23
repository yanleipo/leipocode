# ===========================================================================
# 	DEFINE OUTPUT
# ===========================================================================
OUTPUT         := output.exe

# ===========================================================================
# 	COSMETICS
# ===========================================================================

# For EXAMPLE test.
# ----------------------------------------------------------------------------------
EXAMPLE_SRC    := $(wildcard ./example/*.c)

EXAMPLE_OBJS   := $(patsubst %.c, %.o, $(EXAMPLE_SRC))

EXAMPLE_CFLAGS := -lpthread


# For LIST test.
# -------------------------------------------------------------------------------
LIST_SRC       := $(wildcard ./list/*.c)

LIST_OBJS      := $(patsubst %.c, %.o, $(LIST_SRC))

LIST_CFLAGS    := -DENABLE_DEBUG 

# For FD2FP module
# ---------------------------------------------------------------------------------
export TOOLSET_BASE=/opt/STM/STLinux-2.3/devkit/sh4

FD2FP_DIR      := $(shell pwd)/fd2fp
FD2FP_SRC      := $(FD2FP_DIR)/kernel_fd2fp_test.c
FD2FP_OBJS     := $(patsubst %.c, %.o, $(FD2FP_SRC))
FD2FP_CFLAGS   := -lpthread

# ===========================================================================
# 	BUILD STUFF
# ===========================================================================

# Build help
# ----------
help: 
	@echo "make build_example   - build example code"
	@echo "make clean_example   - clean example code"
	@echo "make build_list      - build linked list code"
	@echo "make clean_list      - clean linked list code"
	@echo "make build_fd2fp     - build fd2fp code"
	@echo "make clean_fd2fp     - clean fd2fp code"

all: help

clean: help

# Build rule for EXAMPLE
# ----------------------
build_example: $(EXAMPLE_OBJS)
	gcc $(EXAMPLE_CFLAGS) $(EXAMPLE_OBJS) -o $(OUTPUT)

clean_example:
	rm -f $(OUTPUT) $(EXAMPLE_OBJS)

$(EXAMPLE_OBJS):%.o:%.c
	gcc $(EXAMPLE_CFLAGS) -c $< -o $@


# Build rule for LIST
# -------------------
build_list: $(LIST_OBJS)
	gcc $(LIST_CFLAGS) $(LIST_OBJS) -o $(OUTPUT)

clean_list:
	rm -f $(OUTPUT) $(LIST_OBJS)

$(LIST_OBJS):%.o:%.c
	gcc $(LIST_CFLAGS) -c $< -o $@


# Build rule for FD2FP
# --------------------
build_fd2fp: $(FD2FP_OBJS)
	@echo "Building fd2fp_module.ko"
	make -C $(FD2FP_DIR) all
	$(if $(TOOLSET_BASE),$(TOOLSET_BASE)/bin/)$(CROSS_COMPILE)gcc $(FD2FP_CFLAGS) $(FD2FP_OBJS) -o $(OUTPUT)

clean_fd2fp:
	make -C $(FD2FP_DIR) clean
	rm -f $(FD2FP_DIR)/Module.symvers
	rm -f $(OUTPUT) $(FD2FP_OBJS)

$(FD2FP_OBJS):%.o:%.c
	$(if $(TOOLSET_BASE),$(TOOLSET_BASE)/bin/)$(CROSS_COMPILE)gcc $(FD2FP_CFLAGS) -c $< -o $@


.PHONY: all clean



