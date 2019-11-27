obj-m:=gpmc_8g.o ram_8g.o gpio_8g.o frontend_8g.o dc_8g.o 
#KERNELDIR:=/opt/EmbedSky/TQ3358/Kernel_3.2_TQ3358_for_Linux_v2.0
KERNELDIR:=/root/newnand/kernel_newnand 
PWD:=$(shell pwd)
all: compile copy
compile:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
copy:
	cp *.ko /mnt/hgfs/work/tftp
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
