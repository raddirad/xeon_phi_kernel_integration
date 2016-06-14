SOURCE = mic.c aes.c
CC = icc

LIBS = -lscif -lrt -liomp5

CFLAGS = -g -O3 -std=gnu99 -openmp

OBJECTS_MIC = mic.o

all: mic

mic: $(OBJECTS_MIC)
	$(CC) $(SOURCE) $(CFLAGS) -mmic -o scale.mic $(LIBS)

*.o: *.c
	$(CC) -c $(CFLAGS) $(LIBS) $(SOURCE)
	
clean:
	rm -f *.o aes_bench*

