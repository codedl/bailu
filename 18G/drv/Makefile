obj-m:=bl_tq3358_gpmc.o bl_tq3358_ram.o  bl_tq3358_gpio.o  bl_tq3358_frontend.o
KERNELDIR:=/opt/EmbedSky/TQ3358/Kernel_3.2_TQ3358_for_Linux_v2.0  
PWD:=$(shell pwd)
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
