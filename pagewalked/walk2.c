#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/slab.h>


MODULE_LICENSE ("Dual BSD/GPL");

#define FIRST_MINOR 10  // minor number should start from 10
#define NR_DEVS 1  // Number of device numbers


int myOpen (struct inode *inode,struct file *filep);
int myRelease (struct inode *in,struct file *fp);
long myIoctl(struct file *fd,unsigned int cmd, unsigned int *x);
void *kmap(struct page *page);


static int myInit (void);
static void myExit (void);


struct file_operations fops = {  // registration to kernel
		.owner = THIS_MODULE,
		.open =	myOpen,
		.unlocked_ioctl = myIoctl,
		.release = myRelease
};

char *devname;
int majNo;
static dev_t mydev;
struct cdev *my_cdev;

module_param (devname, charp, 0000);

static struct class *mychar_class;
static struct device *mychar_device;



int myOpen (struct inode *inode, struct file *filep)
{	
	printk ( "Open successful\n");
	return 0;
}



long myIoctl(struct file *fd, unsigned int cmd, unsigned int *x)
{
	struct task_struct *task=current;

	u32 cr3 ;
	
	pgd_t *my_pgd = task -> mm -> pgd;

	pte_t *my_pte = NULL;

	unsigned int t1 = 0;

	unsigned int *ptr = NULL;

	printk ("newly added one %x\n" , (t1 =  __pa( x ) )) ;

        printk ( "aftrer new kmap %x\n" , ( ptr = kmap(mem_map + (t1 >> 12))) );
        
	printk ( "aftrer new kmap %x\n" , *(unsigned int*)( (unsigned int )ptr + ((int)x & 0x00000fff) ) );
	

	asm volatile ("mov %%cr3 , %0 ":"=r"(cr3) ); /* now here iam moving the value of cr3 register to the variable named cr3 */

	printk(KERN_INFO "cr3 = 0x%8.8X\n", cr3);  /* this is the actual value of cr3 register */

	printk ("my pgd %08x\n", my_pgd); /* this is what i thought to be physical address */
	
	printk ("my pgd2 %08x\n", (unsigned)my_pgd -> pgd); /* this is dereferencing (doesn't make any sense ) */

	my_pgd = my_pgd + ((int)x >> 22);
	
	printk ("mypgd after shift  %08x\n", (unsigned)my_pgd);
	
	printk ("after shift and dereference  %08x\n", (unsigned)(my_pgd -> pgd));

	t1 = (unsigned int)((my_pgd -> pgd) & 0xfffff000);

	if(t1 < 0x38000000) {

		my_pte = (pte_t*)(t1 + 0xC0000000);
		printk ("If1 : %08x\n", (unsigned)my_pte);
	}
	else {
		my_pte = kmap(mem_map + (t1 >> 12));
		printk ("In kmap:%08x\n", (unsigned)my_pte);
	}
	my_pte = my_pte + (((int)x >> 12) & 0x000003ff);
	
	printk ("%08x\n", (unsigned)my_pte);
	
	printk ("%08x\n", (unsigned)my_pte -> pte);

	t1 = (unsigned int)(my_pte -> pte) & 0xfffff000;

	printk ("***********%x\n" , t1) ;

	if(t1 < 0x38000000) {

		ptr = (unsigned int*)(t1 + 0xC0000000);
		printk ("If2 : %08x\n", (unsigned)ptr);
	}
	else {
		ptr = kmap(mem_map + (t1 >> 12));
		printk ("In kmap:%08x\n", (unsigned)ptr);
	}

	ptr = (unsigned int *) ((unsigned)ptr + ((int)x & 0x00000fff));
	
	if(*ptr == *x)

		*ptr = 20;

	return 0;
}



int myRelease (struct inode *in, struct file *fp)
{
	printk (KERN_INFO "File released \n");
	return 0;
}



static int __init myInit (void)
{
	int ret	= -ENODEV;
	int status;

	printk ( "Initializing Character Device \n");

	status	=	alloc_chrdev_region (&mydev, FIRST_MINOR, NR_DEVS, devname);

	if (status < 0)
	{
		printk ( "Device numbers allocation failed: %d \n",status);
		goto err;
	}

	printk ( "Major number allocated = %d \n",MAJOR(mydev));
	my_cdev	=	cdev_alloc ();

	if (my_cdev == NULL) {
	
		printk ( "cdev_alloc failed \n");
		goto err_cdev_alloc;
	}

	cdev_init (my_cdev, &fops);
	my_cdev->owner	=	THIS_MODULE;

	status	=	cdev_add (my_cdev, mydev, NR_DEVS);
	
	if (status) {
	
		printk (KERN_ERR "cdev_add failed \n");
		goto err_cdev_add;
	}

	mychar_class	=	class_create (THIS_MODULE, devname);

	if (IS_ERR(mychar_class)) {
	
		printk ("class_create() failed \n");
		goto err_class_create;
	}

	mychar_device =	device_create (mychar_class, NULL, mydev, NULL, devname);

	if (IS_ERR(mychar_device)) {
	
		printk ("device_create() failed \n");
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
	printk ("Exiting the Character Driver \n");

	device_destroy (mychar_class, mydev);
	class_destroy (mychar_class);
	cdev_del (my_cdev);
	unregister_chrdev_region (mydev, NR_DEVS);

	return;
}

module_init (myInit); // macros provided by the kernel
module_exit (myExit);
