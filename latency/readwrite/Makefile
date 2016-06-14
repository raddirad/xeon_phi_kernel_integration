SOURCE = scif_mic.c
KERNSOURCE = scif_kernel.c
CC = icc

LIBS = -lscif -lrt

CFLAGS = -g -O3 -std=gnu99

OBJECTS_HOST = scif_host.o
OBJECTS_MIC = scif_mic.o

all: host mic

host: $(OBJECTS_HOST)
	$(CC) $(CFLAGS) -o readwrite $(OBJECTS_HOST) $(LIBS)

mic: $(OBJECTS_MIC)
	$(CC) $(SOURCE) $(CFLAGS) -mmic -o readwrite.mic $(LIBS)
kernel: $(OBJECTS_MIC)
	$(CC) $(KERNSOURCE) $(CFLAGS) -mmic -o kernel.mic $(LIBS)

*.o: *.c
	$(CC) -c $(CFLAGS) $(LIBS) $<
	
clean:
	rm -f *.o readwrite *.mic

