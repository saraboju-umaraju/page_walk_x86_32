obj-m:=walk2.o

KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

insert:
	insmod walk2.ko devname=mydev
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	@rm -f Module.symvers
	rmmod walk2
