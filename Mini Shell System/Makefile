CC = gcc
CFLAGS = 
LDFLAGS = 
OBJFILES = myshell.o argparse.o builtin.o
TARGET = myshell

all: $ (TARGET)

$ (TARGET) : $(OBJFILES)
	$(CC) $(CFLAGS) -Wall -Wextra -g -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~