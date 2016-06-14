#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <scif.h>
#include <errno.h>

#define NANOSECONDS 1E9;

void printbuffer(int size, uint8_t* buffer)
{
    printf("buffer content: ");
    for (int i=0;i<size;i+=100000)
    	{ printf("%d, ", buffer[i]); }
    printf("\n");
}

int main( )
{
	size_t len = 1*4096;
	int align = 4096;
	int loops = 50000;
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
	portid.port = 23969;

	ret = scif_connect(endpoint, &portid);
	for( int attempt = 0; ret == -1 && attempt < 10; ++attempt ) 
	{
        	sleep(1);
        	ret = scif_connect(endpoint, &portid);
	//     	printf("attempting connection %d \n", attempt);
	}
	if (ret==-1)
	{ 
		printf("scif_connect failed\n");
		return 1;
	}

	void *ptr;
	ret = posix_memalign((void**)&ptr, align, len);	
	if (ret)
	{
		printf("Allocating memory failed\n");
		return 1;
	}
	memset(ptr, 0, len);

	if( SCIF_REGISTER_FAILED == scif_register(endpoint, ptr, len, (long)ptr, SCIF_PROT_READ | SCIF_PROT_WRITE, SCIF_MAP_FIXED ) )
        {
                printf("scif_register of ptr failed due to: %s\n", strerror(errno));
                return 1;
        }

	void *tempbuffer;
	ret = posix_memalign((void**)&tempbuffer, align, len);
	if (ret)
	{
		printf("Allocating tempbuffer failed\n");
		return 1;
	}

       	if( SCIF_REGISTER_FAILED == scif_register(endpoint, tempbuffer, len, (long)tempbuffer, SCIF_PROT_READ | SCIF_PROT_WRITE, SCIF_MAP_FIXED ) )
        {
               	printf("scif_register of temp failed due to: %s\n", strerror(errno));
                return 1;
        }
//	printf("scif_register of tempbuffer success\n");
	void *outbuffer;
	ret = posix_memalign((void**)&outbuffer, align, len);
	if (ret)
	{
		printf("Allocating outbuffer failed %s\n", strerror(errno));
		return 1;
	}

	if( SCIF_REGISTER_FAILED == scif_register(endpoint, outbuffer, len, (long)outbuffer, SCIF_PROT_READ | SCIF_PROT_WRITE, SCIF_MAP_FIXED ) )
        {
                printf("scif_register of outbuffer failed due to: %s\n", strerror(errno));
                return 1;
        }

	void *remote_ptr;
	void *return_ptr;

	ret = scif_recv(endpoint, &remote_ptr, sizeof(void*), SCIF_RECV_BLOCK);
	if (ret==-1)
	{
		printf("scif_recv failed due to: %s\n", strerror(errno));
		return 1;
	}
//	printf("send_offset0 recieved %p\n", remote_ptr);

        ret = scif_recv(endpoint, &return_ptr, sizeof(void*), SCIF_RECV_BLOCK);
        if (ret==-1)
        {
                printf("scif_recv failed due to: %s\n", strerror(errno));
                return 1;
        }
//	printf("send_offset1 recieved %p\n", return_ptr);

	
	struct timespec start_read, stop_read;
	clock_gettime(CLOCK_REALTIME, &start_read);
	for (int j=1; j<loops; j++)
	{
		if (scif_readfrom(endpoint, (long)ptr, 1, (long)remote_ptr, SCIF_RMA_SYNC))
                {
                        printf("scif_readfrom failed due to: %s\n", strerror(errno));
			return 1;
		}					

	}
	clock_gettime(CLOCK_REALTIME, &stop_read);
	double time_read = (stop_read.tv_sec - start_read.tv_sec) + ( stop_read.tv_nsec - start_read.tv_nsec) / NANOSECONDS;
	double result_read = time_read/loops;
        printf("%1f \n", result_read);

        struct timespec start_write, stop_write;
        clock_gettime(CLOCK_REALTIME, &start_write);
        for (int j=1; j<loops; j++)
        {
                if (scif_writeto(endpoint, (long)ptr, 1, (long)return_ptr, SCIF_RMA_SYNC))
                {
                        printf("scif_writeto failed due to: %s\n", strerror(errno));
                        return 1;
                }

        }
        clock_gettime(CLOCK_REALTIME, &stop_write);
        double time_write = (stop_write.tv_sec - start_write.tv_sec) + ( stop_write.tv_nsec - start_write.tv_nsec) / NANOSECONDS;
        double result_write = time_write/loops;
        printf("%1f \n", result_write);

	ret = scif_send(endpoint, &ptr, sizeof(long), SCIF_SEND_BLOCK);
	if (ret==-1)
	{
		printf("scif_send failed due to: %s\n", strerror(errno));
		return 1;
	}

        ret = scif_unregister(endpoint, (off_t)ptr, len );
        if(ret==-1 && errno!=ENOTCONN )
        {
                printf("scif_unregister failed %s\n", strerror(errno));
                return 1;
        }

	scif_close(endpoint);	

}

