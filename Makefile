


#obj-m:=three.o
#obj-m:=kmap.o
#obj-m:=syscall.o
#include <linux/spinlock.h>
obj-m:=_page_walk.o
#obj-m:=for_each_process.o
#obj-m:=radix_tree.o
#obj-m:=_console.o
#obj-m:=utils.o



KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

i:
	insmod _page_walk.ko devname=mydev
	#insmod kmap.ko devname=mydev
	#insmod for_each_process.ko devname=mydev
	#insmod radix_tree.ko devname=mydev kill=3
	#insmod ps.ko devname=mydev
	#insmod ls_own.ko devname=mydev
	#insmod hello.ko 
	#insmod signal_ioctl.ko
	#insmod syscall.ko
c:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	@rm -f Module.symvers
	dmesg -C
#	rmmod hello
#	rmmod kobjs
#	rmmod for_each_process
#	rmmod radix_tree
#	rmmod ps
#	rmmod ls_own
	rmmod _page_walk
#	rmmod syscall
#	rmmod kmap
	#rmmod signal_ioctl
t:
	dmesg -C ;make c ; make ; make i ; clear ; ./a.out ; dmesg

e:
	@echo $(var)
