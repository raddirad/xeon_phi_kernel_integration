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

void printbuffer(int size, uint8_t* buffer)
{
    printf("buffer content: ");
    for (int i=0;i<size;i+=100000)
    	{ printf("%d, ", buffer[i]); }
    printf("\n");
}

int main( )
{
	size_t len = 536870912;
	int align = 4096;
	scif_epd_t endpoint;
	struct scif_portID portid;
	int ret;
	
	uint8_t *in_key     = malloc(16 * sizeof(uint8_t));
	struct crypto_tfm *tfm
        	= malloc(
                	 sizeof(struct crypto_tfm) +
                 	 sizeof(struct crypto_aes_ctx)
               		 );
	struct crypto_aes_ctx *ctx
        	= crypto_tfm_ctx(tfm);

	ctx->key_length = AES_KEYSIZE_256;
	crypto_aes_set_key(tfm, in_key, AES_KEYSIZE_256);
	

	void *ptr;
	ret = posix_memalign((void**)&ptr, align, len);	
	if (ret)
	{
		printf("Allocating memory failed\n");
		return 1;

	}
	memset(ptr, 0, len);

	void *tempbuffer;
	ret = posix_memalign((void**)&tempbuffer, align, len);
	if (ret)
	{
		printf("Allocating tempbuffer failed\n");
		return 1;
	}

	void *outbuffer;
	ret = posix_memalign((void**)&outbuffer, align, len);
	if (ret)
	{
		printf("Allocating outbuffer failed %s\n", strerror(errno));
		return 1;
	}
	
		struct timespec start_enc, stop_enc;
		clock_gettime(CLOCK_REALTIME, &start_enc);

			#pragma omp parallel for 
			for (int k = 0; k<len; k+=16)
				{ aes_encrypt(tfm, (uint8_t*)&tempbuffer[k], (uint8_t*)&ptr[k]); }

		clock_gettime(CLOCK_REALTIME, &stop_enc);
		double time_enc = (stop_enc.tv_sec - start_enc.tv_sec) + ( stop_enc.tv_nsec - start_enc.tv_nsec) / NANOSECONDS;
		double result0 = len/time_enc/1048576;
        	printf("%1f,", result0);

                struct timespec start_for, stop_for;
                clock_gettime(CLOCK_REALTIME, &start_for);

			#pragma omp parallel for
                       	for (int k=0; k<len; k+=16)
                                { aes_decrypt(tfm, (uint8_t*)&outbuffer[k], (uint8_t*)&tempbuffer[k]); }

                clock_gettime(CLOCK_REALTIME, &stop_for);
                double time_for = (stop_for.tv_sec - start_for.tv_sec) + ( stop_for.tv_nsec - start_for.tv_nsec) / NANOSECONDS;
                double result = len/time_for/1048576;
                printf("%1f \n", result);

	free(ptr);
	free(tempbuffer);
	free(outbuffer);
}

