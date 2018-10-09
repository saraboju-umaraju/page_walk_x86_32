#include	<stdio.h>
#include	<fcntl.h>

int a = 10;

int main(void)
{
	int fd;

	fd = open("/dev/mydev", O_RDWR);

	if(fd < 0)

		perror("Unable to open the device\n");
	
	else
		printf("File opened successfully %d\n", fd);

	printf("process id is: %d\naddress of a: %08x\nvalue of 'a' before: %d\n", getpid(), (unsigned)&a, a);
	
	ioctl(fd, getpid(), &a);

	printf("value of 'a' after: %d\n", a);

	close(fd);

	return 0;
}
