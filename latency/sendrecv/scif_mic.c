#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <scif.h>

#define NANOSECONDS 1E9;

int main( )
{
	uint8_t len = 1;
	uint64_t loops = 50000;

	scif_epd_t endpoint;
	struct scif_portID portid;
	int ret;

        endpoint = scif_open( );
	if( endpoint == SCIF_OPEN_FAILED ) 
	{
            printf("scif open failed\n");
            return 1;
        }

	ret = scif_bind(endpoint, 23955);
	if(ret==-1) 
	{
		printf("scif_bind failed");
		return 1;
	}

	portid.node = 0;
	portid.port = 23968;

	ret = scif_connect(endpoint, &portid);
	for( int attempt = 0; ret == -1 && attempt < 10; ++attempt ) 
	{
        	sleep(1);
        	ret = scif_connect(endpoint, &portid);
	}
	if (ret==-1)
	{ 
		printf("scif_connect failed\n");
		return 1;
	}

	uint8_t* buffer = malloc(len * sizeof(uint8_t));

	struct timespec start_latency, stop_latency;
	clock_gettime(CLOCK_REALTIME, &start_latency);
	for (uint64_t i = 0; i<loops; i++)
	{	
		scif_recv(endpoint, (void*)buffer, len, SCIF_RECV_BLOCK);
		scif_send(endpoint, &buffer[0], len, SCIF_SEND_BLOCK);
	}
	clock_gettime(CLOCK_REALTIME, & stop_latency);
	double time_latency = (stop_latency.tv_sec - start_latency.tv_sec) + (stop_latency.tv_nsec - start_latency.tv_nsec) / NANOSECONDS;
	double result = (time_latency/(2*loops));
	printf("%1f\n", result);
	free(buffer);
	scif_close(endpoint);

	

}

