SOURCE = scif_mic.c
CC = icc

LIBS = -lscif -lrt

CFLAGS = -g -O3 -std=gnu99

OBJECTS_HOST = scif_host.o
OBJECTS_MIC = scif_mic.o

all: host mic

host: $(OBJECTS_HOST)
	$(CC) $(CFLAGS) -o sendrecv $(OBJECTS_HOST) $(LIBS)

mic: $(OBJECTS_MIC)
	$(CC) $(SOURCE) $(CFLAGS) -mmic -o sendrecv.mic $(LIBS)

kernel: $(OBJETCS_MIC)
	$(CC) $(SOURCE) $(CFLAGS) -mmic -o kernel.mic $(LIBS)

*.o: *.c
	$(CC) -c $(CFLAGS) $(LIBS) $<
	
clean:
	rm -f *.o sendrecv *.mic

