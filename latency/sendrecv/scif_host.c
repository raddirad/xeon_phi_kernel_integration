#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <scif.h>

#define NANOSECONDS 1E9;

void randombuffer(int lenght, uint8_t *inbuffer)
{
    for (int i = 0; i < lenght; i++)
    {inbuffer[i] = (rand() % 10);}
}

int main( )
{
	uint8_t len = 1;
	uint64_t loops = 50000;
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
	
	ret = scif_bind(endpoint, 23968);
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

	uint8_t *buffer = malloc(len * sizeof(uint8_t));
	randombuffer (len, buffer);

//Time measurement of latency
	struct timespec start_latency, stop_latency;
	clock_gettime(CLOCK_REALTIME, &start_latency);	
	for (uint64_t i = 0; i<loops; i++)
	{
		scif_send(new_endpoint, &buffer[0], len, SCIF_SEND_BLOCK);
		scif_recv(new_endpoint, (void*)buffer, len, SCIF_RECV_BLOCK);
	}
	clock_gettime(CLOCK_REALTIME, &stop_latency);
	double time_latency = (stop_latency.tv_sec - start_latency.tv_sec) + (stop_latency.tv_nsec - start_latency.tv_nsec) / NANOSECONDS;
	printf("Sending and recieving %d uint8 needed %1f seconds\n", loops, time_latency/(2*loops));

	free(buffer);
	scif_close(endpoint); 

	printf("Done\n");
	

}

