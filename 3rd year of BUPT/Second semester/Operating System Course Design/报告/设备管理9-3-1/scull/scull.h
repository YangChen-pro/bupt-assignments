#ifndef _SCULL_H
#define _SCULL_H

struct scull_dev {
        void *data;   
        int quantum;   //the current quantum size
        int qset;    //the current array size
        unsigned long size;
        unsigned int access_key;    //used by sculluid and scullpriv
        unsigned int usage;      //lock the device while using it
        unsigned int new_msg;
        struct scull_dev *next;
};

struct scull_dev scull;

#include <linux/ioctl.h>

#define SCULL_MAJOR 256
#define SCULL_NAME "test_device"
#define DEVICE_FILE "/dev/scull"

#define SCULL_MAGIC SCULL_MAJOR
#define SCULL_RESET _IO(SCULL_MAGIC,0)			// reset the data
#define SCULL_QUERY_NEW_MSG _IO(SCULL_MAGIC,1)		// check for new message
#define SCULL_QUERY_MSG_LENGTH _IO(SCULL_MAGIC,2)	// get message length
#define IOC_NEW_MSG 1

#endif

