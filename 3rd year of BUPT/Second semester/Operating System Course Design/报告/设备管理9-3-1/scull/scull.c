#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/version.h>
#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/string.h>

#include <asm/io.h>
#include <asm/segment.h>
#include <asm/uaccess.h>

#include "scull.h"

#define DRIVER_AUTHOR "Teacher Wang"
#define DRIVER_DESC "A sample char device driver"

MODULE_LICENSE("GPL");

static int Device_Open = 0;

int scull_open(struct inode *inode, struct file *filp){
	Device_Open++;
	printk("Char device %s is in open\n", SCULL_NAME);
	try_module_get(THIS_MODULE);
	return 0;
}

ssize_t scull_write(struct file *filp, const char *buffer, size_t count, loff_t *off){
	if(count < 0)
		return -EINVAL;
	if(scull.usage || scull.new_msg)
		return -EBUSY;
	scull.usage = 1;
        kfree(scull.data);
	scull.data = kmalloc(sizeof(char) * (count + 1), GFP_KERNEL);
	if(!scull.data){
		return -ENOMEM;
	}
	while(copy_from_user(scull.data, buffer, count+1) != 0);
	scull.usage=0;
	scull.new_msg=1;
	return count;
}

ssize_t scull_read(struct file *filp, char *buffer, size_t count, loff_t *off){
	int length;
	if(count < 0)
		return -EINVAL;
	if(scull.usage)
		return -EBUSY;
	scull.usage=1;
	if(count == 0)
		return 0;
	length = strlen(scull.data);
	if(length < count)
		count = length;
	while(copy_to_user(buffer, scull.data, count+1) != 0);
	scull.new_msg = 0;
	scull.usage = 0;
	return count;
}


int scull_release(struct inode *inode, struct file *filp){
	Device_Open--;
	printk("Char device %s is in release\n", SCULL_NAME);
	module_put(THIS_MODULE);

	return 0;
}

int scull_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,unsigned long arg){
	switch(cmd){
		case SCULL_RESET:
			kfree(scull.data);
			scull.data = NULL;
			scull.usage = 0;
			scull.new_msg = 0;			
			break;
		case SCULL_QUERY_NEW_MSG:
			if(scull.new_msg)
				return IOC_NEW_MSG;
			break;
		case SCULL_QUERY_MSG_LENGTH:
			if(scull.data == NULL) {
				return 0;
			} else {
				return strlen(scull.data);
			}
			break;
		default:
			return -ENOTTY;
	}

	return 0;
}

struct file_operations scull_chops={
	read: scull_read,
	write: scull_write,
	unlocked_ioctl: scull_ioctl,
	open: scull_open,
	release: scull_release
};


int init_module(){
	int result;
	printk(KERN_ALERT"Initializing char device %s.\n", SCULL_NAME);
	result=register_chrdev(SCULL_MAJOR, SCULL_NAME, &scull_chops);
	if(result < 0){
		printk("Scull: Can't get major number!\n");
		return result;
	} else {
		printk("Scull: major number %d.\n", SCULL_MAJOR);
	}

	return 0;
}

void cleanup_module(){
	unregister_chrdev(SCULL_MAJOR, SCULL_NAME);
	printk("Cleanup char device %s.\n", SCULL_NAME);
}

MODULE_LICENSE("GPL" );
MODULE_AUTHOR(DRIVER_AUTHOR );
MODULE_DESCRIPTION(DRIVER_DESC );
MODULE_SUPPORTED_DEVICE("testdevice" );

