CC = icc
CFLAGS = -O2 -openmp
COUNT = -DSTREAM_ARRAY_SIZE=64000000

all: stream_l stream_m

stream_l: stream.c
	$(CC) $(CFLAGS) stream.c -o stream_l -liomp5

stream_m: stream.c
	$(CC) $(CFLAGS) $(COUNT) -mmic stream.c -o stream_m -liomp5

clean:
	rm -f stream_l stream_m *.o
