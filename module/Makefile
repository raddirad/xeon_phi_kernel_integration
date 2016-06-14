KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
MICSYMBOL=/home/radtke/mpss-3.2.4/src/mpss-modules-3.2.4/Module.symvers
KBUILD_EXTRA_SYMBOLS=$(MICSYMBOL)

obj-m :=scaling.o

all:
	$(MAKE) -C $(KERNELDIR) $(KBUILD_EXTRA_SYMBOLS) M=$(PWD) modules
 
clean:
	make -C $(KERNELDIR) M=$(PWD) clean
