obj-m += hooky_4.o
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) 
clean:
	rm *.mod.c *.o *.ko *.symvers *.order

