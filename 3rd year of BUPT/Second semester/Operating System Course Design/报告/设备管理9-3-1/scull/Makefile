obj-m+=scull.o
KDIR=/usr/src/kernels/$(shell uname -r)
PWD:=$(shell pwd)

scull.o: scull.c scull.h
	@$(MAKE) -C $(KDIR) M=${PWD} modules
	@echo ""
	@echo "Run \"insmod scull.ko\" to install scull device."
	@echo "Run \"mknod DEVICE_FILE c MAJOR MINOR\" to make a device file."
	@echo ""

clean:
	@rm -f *.ko *.o *.mod.c *.symvers *.order *.unsigned scull_test

scull_test: scull_test.c scull.h
	@gcc -o scull_test scull_test.c

.PHONY: clean

