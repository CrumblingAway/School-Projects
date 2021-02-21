CXX = g++
CXXFLAGS = -c -std=c++0x -Wall -g
LFLAGS = -std=c++0x -Wall -g

FractalDrawer: Fractal.o FractalDrawer.o
	$(CXX) $(LFLAGS) Fractal.o FractalDrawer.o -o FractalDrawer -lm

FractalDrawer.o: FractalDrawer.cpp
	$(CXX) $(CXXFLAGS) FractalDrawer.cpp -o FractalDrawer.o 

tar: Fractal.cpp Fractal.h FractalDrawer.cpp Makefile README
	tar -cvf cpp_ex2.tar Fractal.cpp Fractal.h FractalDrawer.cpp Makefile README

run:
	make clean
	make FractalDrawer

clean:
	rm -f FractalDrawer *.o	
