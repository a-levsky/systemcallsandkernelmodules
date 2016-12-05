obj-m := find_process.o
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD)
clean:
	rm *.mod.* *.o *.ko *.symvers *.order

