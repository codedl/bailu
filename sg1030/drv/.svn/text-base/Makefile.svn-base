obj-m += uart.o
obj-m += bl_ompl138_gpio.o

TARGET = uart.ko bl_ompl138_gpio.ko

EXEC_DIR=/mnt/hgfs/TftpdWin
LINUXKERNEL_INSTALL_DIR=/opt/SEED-DEC138_kernel
MVTOOL_PREFIX=/opt/Toolchain/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-
MAKE_ENV = ARCH=arm CROSS_COMPILE=$(MVTOOL_PREFIX)
KBUILD_EXTRA_SYMBOLS=/opt/linux_kernel/Module.symvers

debug: EXTRA_CFLAGS += -D__DEBUG

.PHONY: clean debug release install default

default: release
all:release

docs:

release:
	make -C $(LINUXKERNEL_INSTALL_DIR) M=`pwd` $(MAKE_ENV) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules

debug:
	make -C $(LINUXKERNEL_INSTALL_DIR) M=`pwd` $(MAKE_ENV) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules

install:
	install -d $(EXEC_DIR)
	install $(TARGET) $(EXEC_DIR)

clean:
	-rm -rf *.o *mod* *.ko .cmem* .tmp* Module.*
