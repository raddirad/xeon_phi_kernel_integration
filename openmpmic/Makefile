SOURCE = scif_mic.c aes.c
KERNELSOURCE = scif_kernel.c aes.c
SINGLE = mic.c aes.c
CC = icc

LIBS = -lscif -lrt -liomp5

CFLAGS = -g -O3 -std=gnu99 -openmp

OBJECTS_HOST = scif_host.o
OBJECTS_MIC = scif_mic.o
OBJECTS_KERNEL = kernel_mic.o

all: host mic

host: $(OBJECTS_HOST)
	$(CC) $(CFLAGS) -o aes_bench $(OBJECTS_HOST) $(LIBS)

mic: $(OBJECTS_MIC)
	$(CC) $(SOURCE) $(CFLAGS) -mmic -o aes_bench.mic $(LIBS)
kernel: $(OBJECTS_MIC)
	$(CC) $(KERNELSOURCE) $(CFLAGS) -mmic -o kernel.mic $(LIBS)

openmpmic:
	$(CC) $(SINGLE) $(CFLAGS) -mmic -o mic.mic $(LIBS)

*.o: *.c
	$(CC) -c $(CFLAGS) $(LIBS) $(SOURCE)
	
clean:
	rm -f *.o aes_bench* kernel.mic mic.mic

