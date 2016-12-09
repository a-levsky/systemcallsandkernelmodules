obj-m := print_group_lkm.o
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD)
clean:
	rm *.mod.c *.o *.ko *.symvers *.order

