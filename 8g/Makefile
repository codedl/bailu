.PHONY: all clean
exclude_dirs := Makefile
dirs := $(shell ls)
dirs := $(filter-out $(exclude_dirs), $(dirs))
all:	compile
clean:
	$(foreach N, $(dirs), make -C $(N) clean;)
compile:
	$(foreach N, $(dirs), make -C $(N);)
drv_cp:
	cp ./drv/*.ko /mnt/hgfs/work/tftp
