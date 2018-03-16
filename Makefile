CC = gcc
PR = -g

all: halloc.o
	$(CC) $(PR) halloc.o -o start

halloc.o: halloc.c
	$(CC) $(PR) -c halloc.c -o halloc.o

run: all
	./start

clean:
	rm -f *.o start
