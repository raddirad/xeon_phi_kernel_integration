SOURCE = memory.c
SOURCE1 = formem.c
CC = icc

LIBS = -lscif -lrt -liomp5

CFLAGS = -g -O3 -std=gnu99 -openmp

OBJECTS = memory.o
OBJECTS1 = formem.o

all: memcpy formem

memcpy: $(OBJECTS)
	$(CC) $(SOURCE) $(CFLAGS) -mmic -o memory.mic $(LIBS)
formem: $(OBJECTS1)
	$(CC) $(SOURCE1) $(CFLAGS) -mmic -o formem.mic $(LIBS)


*.o: *.c
	$(CC) -c $(CFLAGS) $(LIBS) $(SOURCE)
	
clean:
	rm -f *.o memory.mic formem.mic

