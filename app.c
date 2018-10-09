/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */

#include	<stdio.h>

//#include	"ioctl_head.h"

#include	<fcntl.h>

const int val = 12 ;

int norm = 12 ;

int main(void)
{
	int fd;

	unsigned long x = &norm ;

	/*FILE *fp = fopen ("somefile2" , "r");

	perror("");

	while ( fgetc(fp) != EOF ) ;*/

	fd = open("/dev/mydev", O_RDWR | O_CREAT);

	if(fd < 0)

		perror("Unable to open the device\n");
	
	else
		printf("File opened successfully %d\n", fd);

//	ioctl( fd , getpid() , fp );
//	perror("ioctl");
//	ioctl( fd , getpid() , fp -> _IO_read_ptr );
//	perror("ioctl");
	ioctl( fd , norm ,  x );
	perror("ioctl");
	
	printf ("afret %d\n" , norm);

	
	perror("ioctl");
	

	close(fd);

//	system ("rm /dev/mydev");
	
	return 0;
}
