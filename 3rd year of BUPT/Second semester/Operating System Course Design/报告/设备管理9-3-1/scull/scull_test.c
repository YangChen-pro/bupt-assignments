#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "scull.h"

void write_proc(void);
void read_proc(void);

int main(int argc, char **argv) {
	if(argc == 1) {
		puts("Usage: scull_test [write|read]");
		exit(0);
	}

	if(!strcmp(argv[1], "write")) {
		write_proc();
	}
	else if(!strcmp(argv[1], "read")) {
		read_proc();
	}
	else {
		puts("scull_test: invalid command!");
	}

	return 0;
}

void write_proc() {
	int fd, len, quit = 0;
	char buf[100];
	fd = open(DEVICE_FILE, O_WRONLY);
	if(fd <= 0) {
		printf("Error opening device file %s for writing!\n", DEVICE_FILE);
		exit(1);
	}
	printf("input 'exit' to exit!");
	while(!quit) {
		printf("\n write>>   ");
		fgets(buf, 100, stdin);
		if(!strcmp(buf, "exit\n"))
			quit = 1;
		while(ioctl(fd, SCULL_QUERY_NEW_MSG))
			usleep(1000);
		len=write(fd, buf, strlen(buf));
		if(len < 0) {
			printf("Error writing to device %s!\n", SCULL_NAME);
			close(fd);
			exit(1);
		}
		printf("%d bytes written to device %s!\n", len - 1, SCULL_NAME);
	}
	close(fd);
}

void read_proc() {
	int fd, len, quit = 0;
	char *buf = NULL;
	fd = open(DEVICE_FILE, O_RDONLY);
	if(fd<0) {
		printf("Error opening device file %s for reading!\n", DEVICE_FILE);
		exit(1);
	}
	while(!quit) {
		printf("\n read<<   ");
		while(!ioctl(fd, SCULL_QUERY_NEW_MSG))
			usleep(1000);
		// get the msg length
		len=ioctl(fd, SCULL_QUERY_MSG_LENGTH, NULL);
		if(len) {
			if(buf!=NULL)
				free(buf);
			buf = malloc(sizeof(char) * (len+1));
			len = read(fd, buf, len);
			if(len < 0) {
				printf("Error reading from device %s!", SCULL_NAME);
			}
			else {
				if(!strcmp(buf, "exit\n")) {
					ioctl(fd, SCULL_RESET);     // reset
					quit = 1;
					printf("%s\n",buf);
				}
				else 
					printf("%s\n",buf);
			}
		}
	}
	free(buf);
	close(fd);
}
