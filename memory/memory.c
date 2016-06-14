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

	size_t *src;
	posix_memalign((void**)&src, align, len);

	size_t *dest = malloc(len*sizeof(size_t));
	posix_memalign((void**)&dest, align, len);

	size_t sizearray[10] = {64, 1024, 4096, 16384, 131072, 1048576, 134217728, 268435456, 536870912, 1073741824};
	for (int i=0; i<10; i++)
	{
		struct timespec start_for, stop_for;
                clock_gettime(CLOCK_REALTIME, &start_for);

		#pragma omp parallel for
		for (int j=0;j<15;j++)
			{ memcpy(dest, src, sizearray[i]); }
		
		clock_gettime(CLOCK_REALTIME, &stop_for);
                double time_for = (stop_for.tv_sec - start_for.tv_sec) + ( stop_for.tv_nsec - start_for.tv_nsec) / NANOSECONDS;
                double result = sizearray[i]/time_for*15/1048576;
		printf("%1f\n", result);
	}
}
