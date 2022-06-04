DEBUG ?= 1
ifeq ($(DEBUG), 0)
    CCFLAGS=-Wall -g3 -pedantic
else
    CCFLAGS=-Wall -g3 -pedantic -D DEBUG_FLAG
endif

CC=gcc

all: matrix

matrix: matrix.c
	$(CC) $(CCFLAGS) -o matrix matrix.c -pthread -lrt

clean:
	rm -f threads
