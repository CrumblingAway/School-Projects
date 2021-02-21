#include <iostream>
#include <cmath>
#include "Fractal.h"

/* This file contains the implementation of four different classes:
 * Parent: Fractal, Derived: SierCarpet, SierSieve, CantorDust*/
 
 
 /* ======= Fractal ======= */
 
/**
 * Default ctor for Fractal, sets all values at their respective "null values"
 */
Fractal::Fractal(): _dimension(0), _patternSize(0)
{
	_pattern = {{'\0'}};
}

/**
 * Ctor that receives the dimension of the fractal
 * @param dimension
 */
Fractal::Fractal(int dimension, int patternSize): _dimension(dimension), _patternSize(patternSize) {}

/**
 * Copy constructor
 * @param other
 */
Fractal::Fractal(const Fractal &other)
{
	_pattern = other._pattern;
	_patternSize = other._patternSize;
	_dimension = other._dimension;
}

/**
 * Destructor
 */
Fractal::~Fractal() = default;

/**
 * Create fractal object based on ID
 * @param id id of fractal
 * @param dimension dimension of fractal
 * @return fractal according to id and dimension
 */
Fractal Fractal::fractalFactory(int id, int dimension)
{
	Fractal fractal;
	
	switch(id)
	{
		case 1:
			fractal = SierCarpet(dimension);
			break;
		case 2:
			fractal = SierSieve(dimension);
			break;
		case 3:
			fractal = CantorDust(dimension);
			break;
		default:
			std::cerr << "Invalid input" << std::endl;
	}
	
	return fractal;
}

/**
 * = operator overload
 * @param other
 * @return
 */
Fractal& Fractal::operator=(const Fractal &other)
= default;

/**
 * Overload for << operator to print relevant fractal.
 * Contains algorithm for creating the fractal
 * @param out stream to print
 * @param fractal fractal holding the data
 * @return stream containing fractal
 */
std::ostream& operator<<(std::ostream& out, const Fractal &fractal)
{
	int size = (int) pow(fractal._patternSize, fractal._dimension);
	
	/* Iterate over the cells of the final fractal and determine if it contains '#' or ' ' */
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			int tempI = i;
			int tempJ = j;
			int tempSize = size;
			
			/* Keep calculating while the scope is bigger than the size of the pattern */
			while(tempSize / fractal._patternSize >= fractal._patternSize)
			{
				if(tempSize > fractal._patternSize)
				{
					tempSize /= fractal._patternSize;
				}
				
				/* If at any point we determine we are in a block that should be empty,
				 * add ' ' and break */
				if(fractal._pattern[tempI / tempSize][tempJ / tempSize] == ' ')
				{
					out << ' ';
					tempI = -1;
					break;
				}
				
				while(tempI >= tempSize)
				{
					tempI -= tempSize;
				}
				while(tempJ >= tempSize)
				{
					tempJ -= tempSize;
				}
			}

			/* Reached a grid of the same size as our pattern, simply add the
			 * appropriate char */
			if(tempI != -1)
			{
				out << fractal._pattern[tempI][tempJ];
			}
		}
		
		out << std::endl;
	}
	
	return out;
}


/* ======= SierSieve ======= */

/**
 * Ctor that receives dimension of fractal
 * @param dimension
 */
SierSieve::SierSieve(int dimension): Fractal(dimension, 2)
{
	_pattern = {{'#', '#'}, {'#', ' '}};
}


/* ======= CantorDust ======= */

/**
 * Ctor that receives dimension of fractal
 * @param dimension
 */
CantorDust::CantorDust(int dimension): Fractal(dimension, 3)
{
	_pattern = {{'#', ' ', '#'}, {' ', ' ', ' '}, {'#', ' ', '#'}};
}


/* ======= SierCarpet ======= */

/**
 * Ctor that receives dimension of fractal
 */
SierCarpet::SierCarpet(int dimension): Fractal(dimension, 3)
{
	_pattern = {{'#', '#', '#'}, {'#', ' ', '#'}, {'#', '#', '#'}};
}