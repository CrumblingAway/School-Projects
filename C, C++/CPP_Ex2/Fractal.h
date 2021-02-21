#ifndef CPP_EX2_FRACTAL_H
#define CPP_EX2_FRACTAL_H

#include <iostream>
#include <vector>

/* This file contains the declaration of four different classes:
 * Parent: Fractal, Derived: SierCarpet, SierSieve, CantorDust*/

 
 
/* ======= Fractal ======= */

/**
 * Parent class for fractals
 */
class Fractal
{
	public:

		/**
		 * Default ctor for Fractal, sets all values at their respective "null values"
		 */
		explicit Fractal();
	
		/**
		 * Ctor that receives the dimension of the fractal
		 * @param dimension
		 */
		explicit Fractal(int dimension, int patternSize);
		
		/**
		 * Copy constructor
		 * @param other
		 */
		Fractal(const Fractal &other);
		
		/**
		* Destructor
		*/
		virtual ~Fractal();
	
		/**
* Create fractal object based on ID
* @param id id of fractal
* @param dimension dimension of fractal
* @return fractal according to id and dimension
*/
		static Fractal fractalFactory(int id, int dimension);
		
		/**
		 * = operator overload
		 * @param other
		 * @return
		 */
		Fractal& operator=(const Fractal &other);
		
		/**
		* Overload for << operator to print relevant fractal
		* @param out stream to print
		* @param fractal fractal holding the data
		* @return stream containing fractal
		*/
		friend std::ostream& operator<<(std::ostream& out, const Fractal &fractal);

	protected:
		std::vector<std::vector<char>> _pattern;
		int _dimension;
		int _patternSize;
};

/* ======= SierSieve ======= */

/**
 * Fractal representing the Sierpinsky Sieve
 */
class SierSieve: public Fractal
{
public:
	/**
	 * Ctor that receives dimension of fractal
	 * @param dimension
	 */
	explicit SierSieve(int dimension);

};

/* ======= CantorDust ======= */

/**
 * Fractal representing the Cantor Dust fractal
 */
class CantorDust: public Fractal
{
public:
	
	/**
	 * Ctor that receives dimension of fractal
	 * @param dimension
	 */
	explicit CantorDust(int dimension);
};

/* ======= SierCarpet ======= */

/**
 * Fractal representing the Sierpinsky Carpet
 */
class SierCarpet: public Fractal
{
public:
	/**
	 * Ctor that receives dimension of fractal
	 * @param dimension
	 */
	explicit SierCarpet(int dimension);
};

#endif
