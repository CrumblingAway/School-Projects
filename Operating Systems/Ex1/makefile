CC=g++
CFLAGS= -Wall -g -std=c++11 

all: libosm.a

Ex1: main.o libosm.a 
	$(CC) $(CFLAGS) -o Ex1 main.o libosm.a

main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp -o main.o
	
osm.o: osm.cpp osm.h
	$(CC) $(FLAGS) -c osm.cpp -o osm.o

libosm.a: osm.o osm.h
	ar rs libosm.a osm.o
	
tar: osm.h osm.cpp main.cpp README Makefile
	tar -cvf Ex1.tar osm.h osm.cpp main.cpp README Makefile
	
clean:
	rm -f Ex1 *.o *.a


