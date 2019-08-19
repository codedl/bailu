.PHONY: all clean
exclude_dirs := Makefile
#inc = include
dirs := $(shell ls)
dirs := $(filter-out $(exclude_dirs), $(dirs))
#dirs := $(filter-out $(inc), $(dirs))
#dirs := $(filter-out ssh, $(dirs))
all:
	$(foreach N, $(dirs), make -C $(N);)
clean:
	$(foreach N, $(dirs), make -C $(N) clean;)
