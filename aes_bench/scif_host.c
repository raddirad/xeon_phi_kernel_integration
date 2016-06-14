#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <scif.h>
#include <errno.h>
#include "aes.h"

#define NANOSECONDS 1E9;

void randombuffer(int size, uint8_t *buffer)
{
	for (int i = 0; i < size; i++)
	{ buffer[i] = (rand() % 10); }
}

void printbuffer(int size, uint8_t *buffer)
{
	printf("buffer content: ");
	for (int i=0;i<size;i+=100000)
	{ printf("%d, ", buffer[i]); }
	printf("\n");
}

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
	
	ret = scif_bind(endpoint, 23954);
	if (ret == -1)
	{
		printf("scif_bind failed");
		return 1;
	}
	printf("scif_bind sucessfull\n");

	scif_epd_t new_endpoint;
	ret = scif_listen(endpoint, 4);
	if (ret == -1)
	{
		printf("scif_listen failed\n");
		return 1;
	}
	printf("scif_listen successfull\n");
	scif_accept(endpoint, &portid, &new_endpoint, SCIF_ACCEPT_SYNC);
	if (ret == -1)
	{
		printf("scif_accept failed\n");
		return 1;
	}
	printf("scif_accept successfull\n");

	void *placeholder;
	uint8_t *ptr;
	ret = posix_memalign((void**)&ptr, align, len);
	if (ret)
	{ 
		printf("Allocating memory failed\n"); 
		return 1;
	}
	printf("Allocating Memory successfull\n");

        uint8_t *return_ptr;
       	ret = posix_memalign((void**)&return_ptr, align, len);
        if (ret)
        {
               	printf("Allocating memory for return value failed\n");
                return 1;
        }
     	printf("Allocating memory for return value successfull\n");

	randombuffer(len, ptr);

	if(SCIF_REGISTER_FAILED == scif_register(new_endpoint, ptr, len, (long)ptr, SCIF_PROT_READ | SCIF_PROT_WRITE, SCIF_MAP_FIXED))
	{
		printf("scif_register failed due to: %s\n", strerror(errno));
		return 1;
	}
	printf("scif_register successfull\n");

        if(SCIF_REGISTER_FAILED == scif_register(new_endpoint, return_ptr, len, (long)return_ptr, SCIF_PROT_READ | SCIF_PROT_WRITE, SCIF_MAP_FIXED))
        {
               	printf("scif_register of return_ptr failed due to: %s\n", strerror(errno));
                return 1;
        }
        printf("scif_register of return_ptr successfull\n");
	
	ret = scif_send(new_endpoint, &ptr, sizeof(long), SCIF_SEND_BLOCK);
	if(ret == -1)
	{
		printf("scif_send failed due to: %s\n", strerror(errno));
		return 1;
	}
	printf("scif_send of ptr successfull\n");

	ret = scif_send(new_endpoint, &return_ptr, sizeof(long), SCIF_SEND_BLOCK);
	if (ret == -1)
	{
		printf("sending return value pointer failed due to: %s\n", strerror(errno));
		return 1;
	}
	printf("scif_send of return value pointer successfull\n");
	

	printf("waiting for MIC Code to be finished\n");

	ret = scif_recv(new_endpoint, placeholder, sizeof(long), SCIF_RECV_BLOCK);
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

	ret = scif_unregister(new_endpoint, (off_t)return_ptr, len);
	if (ret == -1)
	{
		printf("scif_unregister for return_ptr failed due to: %s\n", strerror(errno));
		return 1;
	}
	printf("scif unregister for return_ptr successfull\n");

	ret = memcmp(ptr, return_ptr, len);
	if (ret != 0)
	{ printf("Buffers aren't the same.\n"); }
	else
	{ printf("Buffers are the same.\n"); }

	sleep(2);
	scif_close(endpoint); 
	scif_close(new_endpoint);

	printf("Done\n");
	

}

