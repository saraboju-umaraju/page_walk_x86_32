
/** System Includes **/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/page.h>
#include<asm/pgtable_32.h>
#include<asm/pgtable.h>
#include<asm/highmem.h>
MODULE_LICENSE ("Dual BSD/GPL");

/** Constants **/
#define FIRST_MINOR	10
#define NR_DEVS 1	//	Number of device numbers

// Function Declaration for syscall definitions
int myOpen (struct inode *inode,struct file *filep);
int myRelease (struct inode *in,struct file *fp);
long myIoctl (struct file *fp,unsigned int pid,unsigned long int addr);

//Initialization routines
static int myInit (void);
static void myExit (void);

struct file_operations fops = {
		.owner	        =	THIS_MODULE,
		.open	        =	myOpen,
                .unlocked_ioctl =   	myIoctl,
		.release	=    	myRelease,
};
/* Global varibales */
char *devname; // Contains device name
int majNo;
static dev_t mydev; // encodes major number and minor number
struct cdev *my_cdev; // holds character device driver descriptor

/* To accept input from the command line */
module_param (devname, charp, 0000);

// Class and device structures 
static struct class *mychar_class;
static struct device *mychar_device;

/*
 * myopen: open fucntions of the pseudo driver
 *
 */

int myOpen (struct inode *inode, struct file *filep)
{	
	printk (KERN_INFO "Open successful\n");
	return 0;
}

/*
 * myRelease: close function of the pseduo driver
 *
 */

int myRelease (struct inode *in, struct file *fp)
{
	printk (KERN_INFO "File released \n");
	return 0;
}

/*
 * myIoctl: function for doing control operations on driver
 *
 */

long myIoctl (struct file *fp,unsigned int pid, unsigned long int addr)
{
	pgd_t *my_pgd = NULL;
	pmd_t *my_pmd = NULL;
	pte_t *my_pte = NULL;
	void *my_page = NULL;
	
	my_pgd = current->mm->pgd;	//pointing to pgd base addr
	my_pgd += (addr >> 30);		// adding offset to pgd
	
	my_pmd = kmap(mem_map + ((my_pgd->pgd) >> 12)); //VA of PMD	

	my_pmd += ((addr & 0x3fe00000) >> 21);// adding offset to pmd

	my_pte = kmap(mem_map + ((my_pmd->pmd) >> 12)); //VA of PTE
	my_pte +=((addr & 0x001ff000) >>12); // adding offset to PTE

	my_page = kmap(mem_map + ((my_pte->pte) >> 12)); //VA of page
	addr = (addr & 0x00000fff);
	my_page = (unsigned long *) ( (char *)my_page + addr );
	
	*((int *)my_page) = 20;    
	return 0;
}

/*
 * myInit: init function of the kernal module
 *
 */

static int __init myInit (void)
{
	int ret	=	-ENODEV;
	int status;

	printk (KERN_INFO "Initializing Character Device \n");

    // Allocating Device Numbers
	status	=	alloc_chrdev_region (&mydev, FIRST_MINOR, NR_DEVS, devname);
    if (status < 0)
    {
	    printk (KERN_NOTICE "Device numbers allocation failed: %d \n",status);
        goto err;
    }

    printk (KERN_INFO "Major number allocated = %d \n",MAJOR(mydev));
    my_cdev	=	cdev_alloc (); // Allocate memory for my_cdev
    if (my_cdev == NULL) {
	    printk (KERN_ERR "cdev_alloc failed \n");
	    goto err_cdev_alloc;
}

    cdev_init (my_cdev, &fops); // Initialize my_cdev with fops
    my_cdev->owner	=	THIS_MODULE;

    status	=	cdev_add (my_cdev, mydev, NR_DEVS); // Add my_cdev to the list
    if (status) {
	printk (KERN_ERR "cdev_add failed \n");
	goto err_cdev_add;
}
    // Create a class an entry in sysfs
    mychar_class = class_create (THIS_MODULE, devname);
    if (IS_ERR(mychar_class)) {
	printk (KERN_ERR "class_create() failed \n");
	goto err_class_create;
}
    // Creates mychar_device in sysfs and an
    // device entry wiil be made in /dev directory
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
/*
 * myExit: cleanup function of the kernal module
 */

static void myExit (void)
{
	printk (KERN_INFO "Exiting the Character Driver \n");

	device_destroy (mychar_class, mydev);
	class_destroy (mychar_class);
	cdev_del (my_cdev);
	unregister_chrdev_region (mydev, NR_DEVS);

	return;
}

module_init(myInit);
module_exit(myExit);

