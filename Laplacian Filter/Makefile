DEBUG ?= 1
ifeq ($(DEBUG), 0)
    CCFLAGS=-Wall -g3 -pedantic
else
    CCFLAGS=-Wall -g3 -pedantic -D DEBUG_FLAG
endif

CC=gcc

all: imath

imath: imath.c 
	$(CC) $(CCFLAGS) -o imath imath.c -pthread

clean:
	rm -f imath