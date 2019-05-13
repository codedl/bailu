obj-m:=gpmc_18g.o ram_18g.o  gpio_18g.o  frontend_18g.o 
#KERNELDIR:=/opt/EmbedSky/TQ3358/Kernel_3.2_TQ3358_for_Linux_v2.0
KERNELDIR:=/root/newnand/kernel_newnand 
PWD:=$(shell pwd)
all:compile copy
compile:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
copy:
	cp *.ko /mnt/hgfs/work/tftp
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
