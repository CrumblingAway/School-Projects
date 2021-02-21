CC=g++
CFLAGS= -Wall -g -std=c++11 

all: libuthreads.a

Ex2: main.o libuthreads.a
	$(CC) $(CFLAGS) -o Ex2 main.o libuthreads.a

main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp -o main.o
	
uthreads.o: uthreads.cpp uthreads.h
	$(CC) $(FLAGS) -c uthreads.cpp -o uthreads.o

Thread.o: Thread.cpp Thread.h
	$(CC) $(FLAGS) -c Thread.cpp Thread.h

libuthreads.a: uthreads.o uthreads.h Thread.o
	ar rs libuthreads.a uthreads.o Thread.o

tar: uthreads.h uthreads.cpp main.cpp README Makefile
	tar -cvf Ex1.tar uthreads.h uthreads.cpp Thread.cpp Thread.h main.cpp README Makefile
	
clean:
	rm -f Ex2 *.o *.a


