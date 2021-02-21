CXX = g++
CXXFLAGS = -c -std=c++0x -Wall -g
LFLAGS = -std=c++0x -Wall -g

IntegerFactorization: IntegerFactorization.o GField.o GFNumber.o
	$(CXX) $(LFLAGS) IntegerFactorization.o GField.o GFNumber.o -o IntegerFactorization -lm

IntegerFactorization.o: IntegerFactorization.cpp
	$(CXX) $(CXXFLAGS) IntegerFactorization.cpp -o IntegerFactorization.o 

GField.o: GField.cpp
	$(CXX) $(CXXFLAGS) GField.cpp -o GField.o

GFNumber.o: GFNumber.cpp
	$(CXX) $(CXXFLAGS) GFNumber.cpp -o GFNumber.o

tar: GField.cpp GField.h GFNumber.cpp GFNumber.h IntegerFactorization.cpp Makefile README
	tar -cvf cpp_ex1.tar GField.cpp GField.h GFNumber.cpp GFNumber.h IntegerFactorization.cpp Makefile README

run:
	make clean
	make IntegerFactorization

clean:
	rm -f IntegralFactorization *.o	
