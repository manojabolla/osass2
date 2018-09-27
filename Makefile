CC     = gcc
CFLAGS = -g
TARGET = master worker
all: $(TARGET)
OBJS   = master.o worker.o
#.SUFFIXES: .c .o

master: master.o
	$(CC) -o master master.o

worker: worker.o
	$(CC) -o worker worker.o

.c.o:
	$(CC) $(CFLAGS) -c $<
clean:
	/bin/rm -f *.o $(TARGET)

