/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */

#include	<stdio.h>

//#include	"ioctl_head.h"

#include	<fcntl.h>

const int val = 12 ;

const int norm = 12 ;

int main(void)
{
	int fd;

	unsigned long x = &val ;

	perror("");

	fd = open("/dev/mydev", O_RDWR );

	if(fd < 0)

		perror("Unable to open the device\n");
	
	else
		printf("File opened successfully %d\n", fd);

	if ( 0 == ioctl( fd , val , &val )) {

	}
	
	perror("ioctl");

	printf("after walk %d\n ", val);	

	sleep(3);

	int *p = &val ;

	*p = 123 ;
	
	printf("after walk %d\n ", val);	

	close(fd);

	return 0;
}
