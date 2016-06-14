#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <scif.h>

int main( )
{
	uint8_t len = 200;
	uint64_t loops = 2;

	scif_epd_t endpoint;
	struct scif_portID portid;
	int ret;

        printf("output MIC\n");
        endpoint = scif_open( );
	if( endpoint == SCIF_OPEN_FAILED ) 
	{
            printf("scif open failed\n");
            return 1;
        }
	printf("scif_open successfull\n");

	ret = scif_bind(endpoint, 23955);
	if(ret==-1) 
	{
		printf("scif_bind failed");
		return 1;
	}
	printf("scif_bind successfull\n");

	portid.node = 0;
	portid.port = 23954;

	ret = scif_connect(endpoint, &portid);
	for( int attempt = 0; ret == -1 && attempt < 10; ++attempt ) 
	{
        	sleep(1);
        	ret = scif_connect(endpoint, &portid);
        	printf("attempting connection %d \n", attempt);
	}
	if (ret==-1)
	{ 
		printf("scif_connect failed\n");
		return 1;
	}
	printf("scif_connect successfull\n");

	uint8_t* buffer = malloc(len * sizeof(uint8_t));

	ret = scif_recv(endpoint, (void*)buffer, len, SCIF_RECV_BLOCK);
	if (ret != len)
	{
		printf("scif_recv failed\n");
		return 1;
	}
	printf("scif_recv successfull\n");
	printf("received:\n");
	for (int i=0;i<len;i++)
	{ printf("%d, ",buffer[i]); }
	printf("\n");
	printf("sending:\n");
	for (int i=0; i<len; i++)
	{
		buffer[i]=buffer[i]+1;
		printf("%d, ", buffer[i]);
	}
	printf("\n");

	ret = scif_send(endpoint, &buffer[0], len, SCIF_SEND_BLOCK);
	if (ret !=len)
	{
		printf("scif_send failed\n");
		return 1;
	}
	printf("scif_send successfull\n");

	free(buffer);
	scif_close(endpoint);

	printf("Done\n");
	

}

