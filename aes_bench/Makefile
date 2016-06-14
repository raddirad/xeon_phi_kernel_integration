SOURCE = scif_mic.c aes.c
CC = icc

LIBS = -lscif -lrt

CFLAGS = -g -O3 -std=gnu99

OBJECTS_HOST = scif_host.o
OBJECTS_MIC = scif_mic.o

all: host mic

host: $(OBJECTS_HOST)
	$(CC) $(CFLAGS) -o aes_bench $(OBJECTS_HOST) $(LIBS)

mic: $(OBJECTS_MIC)
	$(CC) $(SOURCE) $(CFLAGS) -mmic -o aes_bench.mic $(LIBS)

*.o: *.c
	$(CC) -c $(CFLAGS) $(LIBS) $(SOURCE)
	
clean:
	rm -f *.o aes_bench*

