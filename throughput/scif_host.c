#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <scif.h>
#include<errno.h>

#define NANOSECONDS 1E9;

int main( )
{
	size_t len = 1073741824;
	int align = 4096;
	scif_epd_t endpoint;
	struct scif_portID portid;
	int ret;

        printf("output\n");
        endpoint = scif_open( );
        if( endpoint == SCIF_OPEN_FAILED ) 
	{
            	printf("scif open failed\n");
		return 1;
        }
	printf("scif_open successfull\n");
	
	ret = scif_bind(endpoint, 23962);
	if (ret == -1)
	{
		printf("scif_bind failed");
		return 1;
	}
	printf("scif_bind sucessfull\n");

	scif_epd_t new_endpoint;
	ret = scif_listen(endpoint, 4);
	if (ret==-1)
	{
		printf("scif_listen failed\n");
		return 1;
	}
	printf("scif_listen successfull\n");
	scif_accept(endpoint, &portid, &new_endpoint, SCIF_ACCEPT_SYNC);
	if (ret==-1)
	{
		printf("scif_accept failed\n");
		return 1;
	}
	printf("scif_accept successfull\n");

	uint8_t *ptr;
	ret = posix_memalign((void**)&ptr, align, len);
	if (ret)
	{ 
		printf("Allocating memory failed\n"); 
		return 1;
	}
	printf("Allocating Memory successfull\n");

	if(SCIF_REGISTER_FAILED == scif_register(new_endpoint, ptr, len, (long)ptr, SCIF_PROT_READ | SCIF_PROT_WRITE, SCIF_MAP_FIXED))
	{
		printf("scif_register failed due to: %s\n", strerror(errno));
		return 1;
	}
	printf("scif_register successfull\n");
	sleep(1);

	ret = scif_send(new_endpoint, &ptr, sizeof(long), SCIF_SEND_BLOCK);
	if(ret == -1)
	{
		printf("scif_send failed due to: %s\n", strerror(errno));
		return 1;
	}
	printf("scif_send successfull\n");
	
	printf("waiting for MIC Code to be finished\n");

	ret = scif_recv(new_endpoint, ptr, sizeof(long), SCIF_RECV_BLOCK);
	if (ret==-1)
	{
		printf("scif_recv failed due to: %s\n", strerror(errno));
		return 1;
	}
	printf("scif_recv successfull\n");

	ret = scif_unregister( new_endpoint, (off_t)ptr, len );
    	if( ret == -1 && errno != ENOTCONN )
	{ 
		printf("scif_unregister failed %s\n", strerror(errno)); 
		return 1;
	}
	printf("scif_unregister successfull\n");

	sleep(1);
	scif_close(endpoint); 
	scif_close(new_endpoint);

	printf("Done\n");
	

}

