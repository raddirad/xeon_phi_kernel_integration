#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define NANOSECONDS 1E9;

int main()
{
	size_t len = 1073741824;
	size_t align = 4096;
	uint8_t src[len];
	uint8_t dst[len];

	for (int i=0; i<len; i++)
		{ src[i]=(rand()%100+1); }

	
	size_t sizearray[10] = {64, 1024, 4096, 16384, 131072, 1048576, 134217728, 268435456, 536870912, 1073741824};
	for (int i=0; i<10; i++)
	{
		struct timespec start_for, stop_for;
                clock_gettime(CLOCK_REALTIME, &start_for);
		for (int k=0; k<20; k++)
		{		
			for (int j=0;j<sizearray[i]; j++)
				{ dst[j]=src[j]; }
		}

		clock_gettime(CLOCK_REALTIME, &stop_for);
                double time_for = (stop_for.tv_sec - start_for.tv_sec) + ( stop_for.tv_nsec - start_for.tv_nsec) / NANOSECONDS;
                double result = sizearray[i]/time_for*20/1048576;
		printf("%1f\n", time_for);
	}
}
