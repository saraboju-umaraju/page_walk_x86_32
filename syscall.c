#include <linux/syscalls.h>
#include <linux/errno.h>
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/init.h>
#include <asm/asm-offsets.h>

#include <linux/fs.h>

#include <linux/slab.h>

void **sys_call_table = NULL ;

long asmlinkage no_syscall_copy ( void ) 
{
	return -ENOSYS ;
}

int init_module(void)

{

		printk("********************init module**************");

		sys_call_table = (void**) kallsyms_lookup_name("sys_call_table") ;

		printk ("found sys_call_table at %p\n" , sys_call_table) ;

		int i = 0 ;

		while ( i < __NR_syscall_max ) {

			if ( ! memcmp ( sys_call_table [i] , no_syscall_copy , 17 ) )	{
			
				printk ("sys call %3d is not implemented\n" , i) ;

			}

			i++ ;

		}
	
		return 0;
}


void cleanup_module(void)
{
		
		printk(KERN_ALERT "*************************************************************\n");


		printk(KERN_ALERT "*************************************************************\n");

}  

MODULE_LICENSE("GPL");

