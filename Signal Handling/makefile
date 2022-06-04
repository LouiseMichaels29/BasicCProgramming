DEBUG ?= 1
ifeq ($(DEBUG), 0)
    CCFLAGS=-Wall -g3 -pedantic
else
    CCFLAGS=-Wall -g3 -pedantic -D DEBUG_FLAG
endif

CC=gcc

all: threads

threads: threads.c
	$(CC) $(CCFLAGS) -o threads threads.c -pthread -lrt

clean:
	rm -f threads
