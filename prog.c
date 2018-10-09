#include	<stdio.h>

#include	<signal.h>

#include	<stdio_ext.h>

#include	<string.h>

#include	<fcntl.h>

#include	<stdlib.h>

#include	<sys/mman.h>

	int fd = 0 ;
void _hand() {
	
	printf("in handler %x\n" , _hand);
//            ioctl(fd , fd , fd);

exit(0);
}

int main ( void ) 
{

	if ( ( fd = open ( "/dev/mydev" , O_RDWR )  ) < 0  ) {

		perror("opne");
		
		return 1 ;
	
	}

	
			ioctl(fd , fd , fd);

			perror("ioctl");

		if ( getchar() == -1 )  {
	
	signal(2 , _hand ) ;
	signal(3 , _hand ) ;
}
			ioctl(fd , fd , fd);
		raise(3);
			
		close ( fd ) ;

	return 0 ;

}
