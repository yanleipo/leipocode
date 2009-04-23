# ===========================================================================
# 	BUILD STUFF FOR FD2FP MODULE
# ===========================================================================
export KERNEL_DIR=/mnt/work/softathome/softathome/kernel/linux-sh4-STAPI_2.6.23.17_stm23_A14_0117
ARCH           := sh
CROSS_COMPILE  := sh4-linux-

obj-m := fd2fp_module.o
fd2fp_module-objs := kernel_fd2fp_module.o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(if $(TOOLSET_BASE),$(TOOLSET_BASE)/bin/)$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(shell pwd) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(if $(TOOLSET_BASE),$(TOOLSET_BASE)/bin/)$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(shell pwd) clean
