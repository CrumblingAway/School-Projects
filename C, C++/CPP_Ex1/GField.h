#ifndef CPP_EX1_GFIELD_H
#define CPP_EX1_GFIELD_H

#include <iostream>
#include <cmath>


/* Forward declaration of GFNumber */
class GFNumber;

#ifndef CPP_EX1_GFIELD_H
#include "GFNumber.h"
#endif

/**
 * Class representing a Galois field
 */
class GField
{
private:
	long base;
	long degree;

public:
	/**
	 * Constructor that receives base and degree
	 * @param p base
	 * @param l degree
	 */
	GField(const long &p, const long &l);
	
	/**
	 * Default constructor, sets base = 2 and degree = 1
	 */
	GField();
	
	/**
	 * Constructor that receives base, sets degree = 1
	 * @param p base
	 */
	GField(const long &p);
	
	/**
	 * Copy constructor
	 * @param other GField object to copy
	 */
	GField(const GField &other);
	
	/**
	 * Destructor
	 */
	~GField();
	
	/**
	 * Get char (base) of field
	 * @return
	 */
	long getChar() const;
	
	/**
	 * Get degree of field
	 * @return
	 */
	long getDegree() const ;
	
	/**
	 * Get order of field (base to the power of the degree)
	 * @return
	 */
	long getOrder() const;
	
	/**
	 * Find greatest common divisor of a and b
	 * @param a
	 * @param b
	 * @return gcd of a and b
	 */
	GFNumber gcd(GFNumber a, GFNumber b);
	
	/**
	 * Get adjusted instance of k in field
	 * @param k
	 * @return
	 */
	GFNumber createNumber(long k) const;
	
	/**
	 * Static method for checking if num is prime
	 * @param num number to check
	 * @return true if prime, otherwise false
	 */
	static bool isPrime(long num);
	
	/* Operators */
	
	/**
	 * Set this GField's values as other's values
	 * @param other other GField
	 * @return reference to adjusted GField
	 */
	GField& operator=(GField const &other);
	
	/**
	 * Compares between GFields by order
	 * @param other
	 * @return true if order is same, otherwise false
	 */
	bool operator==(GField const &other) const;
	
	/**
	 * Opposite of == operator
	 * @param other
	 * @return opposite of == result
	 */
	bool operator!=(GField const &other) const;
	
	/**
	 * Create output for printing
	 * @param out stores output
	 * @param field GField object to print
	 * @return the result
	 */
	friend std::ostream& operator<<(std::ostream& out, GField const &field);
	
	/**
	 * Read data into GField object
	 * @param in data
	 * @param field GField object
	 * @return data
	 */
	friend std::istream& operator>>(std::istream& in, GField &field);
};

#endif
