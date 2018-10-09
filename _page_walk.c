#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/ptrace.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/threads.h>
#include <asm/kmap_types.h>
#include <asm/tlbflush.h>
#include <asm/paravirt.h>
#include <asm/fixmap.h>
//#include <asm/_umas.h>
#include <linux/highmem.h>

MODULE_LICENSE ("Dual BSD/GPL");

#define FIRST_MINOR	10
#define NR_DEVS 1	//	Number of device numbers



int myOpen (struct inode *inode,struct file *filep);
int myRelease (struct inode *in,struct file *fp);
long myioctl(struct file *, unsigned int, long unsigned int);
//long myioctl(int xx, unsigned int, long unsigned int);
static int myInit (void);
static void myExit (void);
void *kmap(struct page *page);

struct file_operations fops = { //For required file operations
		.owner	 = THIS_MODULE,
		.open	 = myOpen,
		.release = myRelease,
		.unlocked_ioctl = myioctl
};

char *devname;
int majNo;
static dev_t mydev;
struct cdev *my_cdev;

module_param (devname, charp, 0000);//to accept the command_line arguments

static struct class *mychar_class;
static struct device *mychar_device;


int myOpen (struct inode *inode, struct file *filep)
{	
		printk (KERN_DEBUG "Open successful\n");
		return 0;
}


//long myioctl(int xx, unsigned int x, long unsigned int var)
long myioctl(struct file* filp, unsigned int x, long unsigned int var ) 
{
#if 1
		struct task_struct *task = current;//To get the current process running 

		pte_t *pte_base_addr = NULL; //base address of pte whole page
		pgd_t *pgd_base_addr = NULL;// base address of whole pgd

		pte_t *pte_entry = NULL ; // indexing into pgd , a pgd struct
		pgd_t *pgd_entry = NULL  ;// indexing into pte a pte structure

		int MSB_10bits = 0 ; // offset to indec into pgd or MSB 10 bits
		int middle_10bits = 0 ; // offset into pte middle 10 bits
		int LSB_12bits = 0 ; // last 12 bits

		unsigned long page_address = 0 ;
		unsigned long page_address_offset = 0 ;

		pgprot_t new ;	

		pgd_base_addr  = task -> mm -> pgd ;

		MSB_10bits = var >> 22 ;

		middle_10bits = ( var & 0x003ff000 ) ;

		LSB_12bits = var & 0x00000fff ;

		pgd_entry = ( pgd_t* )((unsigned int*)pgd_base_addr + MSB_10bits) ;

		pte_base_addr = kmap ( mem_map + ( ( (unsigned int) pgd_entry -> pgd ) >> 12 ) ) ;

		pte_entry = (pte_t*) ( (unsigned int*) pte_base_addr + ( ( middle_10bits ) >> 12 ) ) ;

		new . pgprot = 0 ;

		page_address = (unsigned) kmap ( mem_map + ( ( (unsigned int) pte_entry -> pte ) >> 12 ) ) ;

		page_address_offset =  page_address + LSB_12bits ;

		//GPF_DISABLE ;

		//GPF_ENABLE ;


		if ( *(unsigned int*) page_address_offset == x ) {

				printk ("good job page walk is successful\n") ;

				*(unsigned int*) page_address_offset = 199 ;

		}
#endif


#if 0
		{

				int cnt = 0;

				int i = 0;

				int j = 0;

				pgd_t *my_pgd;

				pte_t *my_pte;

				//my_pgd = current -> mm -> pgd ;

				my_pgd = kallsyms_lookup_name("swapper_pg_dir") ;

				printk("pgd = %u pmd = %u pte = %u \n",PTRS_PER_PGD,PTRS_PER_PMD,PTRS_PER_PTE);

				while(i < PTRS_PER_PGD) {

						if( my_pgd -> pgd ) {

								printk("---------PGD[%3d].%08x ----------\n",i++,(unsigned int)my_pgd -> pgd);

								cnt++;

								my_pte = kmap((mem_map + (((my_pgd -> pgd) & PAGE_MASK) >> PAGE_SHIFT)));

								while( j < PTRS_PER_PTE) {

										if( my_pte -> pte ) 

//											printk("\t\t-PTE[%3d].%08x \n",j++,(unsigned int)my_pte -> pte);

										my_pte++;
								}

							//	kunmap ( (mem_map + (((my_pgd -> pgd) & PAGE_MASK) >> PAGE_SHIFT))) ;

								j = 0;

						}
						my_pgd++;
				}
				printk("count = %d \n",cnt);
		}
#endif 
		//	printk("swapper_pg_dir %lx\n" , kallsyms_lookup_name ( "swapper_pg_dir" ));

		//	pte_modify( *pte_base_addr, new ) ;

		//	pte_wrprotect ( *pte_base_addr ) ;

		//ptep_set_wrprotect ( pte_base_addr ) ;

		//	pte_mkwrite ( *pte_base_addr ) ;


#if 0
		struct sighand_struct *hand = NULL ;

		hand = current -> sighand ;

		int i = 0 ;

		struct k_sigaction **action = hand -> action ;

		while ( i < 64 ) {

		if ( action[i]  )
	
		printk("%10x %d \n" , action[i] , i);

		//printk("_NSIG is %x\n" , action[i] -> sa .sa_handler );


		i++ ;
}
		printk("end of an era \n");
		return 0;
#endif
}

int myRelease (struct inode *in, struct file *fp)
{
		printk (KERN_INFO "File released \n");
		return 0;
}

static int __init myInit (void)
{
		int ret	=	-ENODEV;
		int status;

		if ( devname == NULL ) {

				printk ("you've missed command line argument \n");

				return  0;

		}

		printk (KERN_INFO "Initializing Character Device \n");

		status	=	alloc_chrdev_region (&mydev, FIRST_MINOR, NR_DEVS, devname);

		if (status < 0)
		{
				printk (KERN_NOTICE "Device numbers allocation failed: %d \n",status);
				goto err;
		}

		printk (KERN_INFO "Major number allocated = %d \n",MAJOR(mydev));
		my_cdev	=	cdev_alloc ();

		if (my_cdev == NULL) {
				printk (KERN_ERR "cdev_alloc failed \n");
				goto err_cdev_alloc;
		}

		cdev_init (my_cdev, &fops);
		my_cdev->owner	=	THIS_MODULE;

		//The kernel maintains a list of character devices under cdev_map
		status	=	cdev_add (my_cdev, mydev, NR_DEVS); //cdev_add registers a character device with the kernel. 
		if (status) {
				printk (KERN_ERR "cdev_add failed \n");
				goto err_cdev_add;
		}

		mychar_class	=	class_create (THIS_MODULE, devname);
		if (IS_ERR(mychar_class)) {
				printk (KERN_ERR "class_create() failed \n");
				goto err_class_create;
		}

		mychar_device =	device_create (mychar_class, NULL, mydev, NULL, devname);
		if (IS_ERR(mychar_device)) {
				printk (KERN_ERR "device_create() failed \n");
				goto err_device_create;
		}

		return 0;

err_device_create:
		class_destroy (mychar_class);

err_class_create:
		cdev_del(my_cdev);

err_cdev_add:
		kfree (my_cdev);

err_cdev_alloc:

		unregister_chrdev_region (mydev, NR_DEVS);

err:
		return ret;
}


static void myExit (void)
{
		printk (KERN_INFO "Exiting the Character Driver \n");

		device_destroy (mychar_class, mydev);
		class_destroy (mychar_class);
		cdev_del (my_cdev);
		unregister_chrdev_region (mydev, NR_DEVS);

		return;
}

module_init (myInit);
module_exit (myExit);

MODULE_LICENSE ("Dual BSD/GPL");
