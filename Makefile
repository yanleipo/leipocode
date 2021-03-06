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
export CROSS_COMPILE=sh4-linux-

FD2FP_DIR      := $(shell pwd)/fd2fp
FD2FP_SRC      := $(FD2FP_DIR)/kernel_fd2fp_test.c
FD2FP_OBJS     := $(patsubst %.c, %.o, $(FD2FP_SRC))
FD2FP_CFLAGS   := -lpthread

# For POLYMORPHISM test.
# -------------------------------------------------------------------------------
POLY_SRC       := $(wildcard ./polymorphism/*.c)

POLY_OBJS      := $(patsubst %.c, %.o, $(POLY_SRC))

POLY_CFLAGS    := 

# For DISPLAY_BINARY test.
# -------------------------------------------------------------------------------
DISPLAY_BINARY_SRC       := $(wildcard ./display_binary/*.c)

DISPLAY_BINARY_OBJS      := $(patsubst %.c, %.o, $(DISPLAY_BINARY_SRC))

DISPLAY_BINARY_CFLAGS    := 

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
	@echo "make build_poly      - build poly code"
	@echo "make clean_poly      - clean poly code"
	@echo "make build_display_binary    - build display_binary code"
	@echo "make clean_display_binary    - clean display_binary code"

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

# Build rule for POLYMORPHISM
# ---------------------------
build_poly: $(POLY_OBJS)
	gcc $(POLY_CFLAGS) $(POLY_OBJS) -o $(OUTPUT)

clean_poly:
	rm -f $(OUTPUT) $(POLY_OBJS)

$(POLY_OBJS):%.o:%.c
	gcc $(POLY_CFLAGS) -c $< -o $@

# Build rule for DISPLAY_BINARY 
# -----------------------------
build_display_binary: $(DISPLAY_BINARY_OBJS)
	gcc $(DISPLAY_BINARY_CFLAGS) $(DISPLAY_BINARY_OBJS) -o $(OUTPUT)

clean_display_binary:
	rm -f $(OUTPUT) $(DISPLAY_BINARY_OBJS)

$(DISPLAY_BINARY_OBJS):%.o:%.c
	gcc $(DISPLAY_BINARY_CFLAGS) -c $< -o $@


.PHONY: all clean



