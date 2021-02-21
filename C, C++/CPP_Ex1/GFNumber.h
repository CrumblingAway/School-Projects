#ifndef CPP_EX1_GFNUMBER_H
#define CPP_EX1_GFNUMBER_H

#include <iostream>
#include "GField.h"

/**
 * Class representing a number in a given Galois field
 */
class GFNumber
{
private:
	long num;
	GField field;

public:
	/**
	 * Constructor that receives a number and a field
	 */
	GFNumber(long n, GField newField);
	
	/**
	 * Default constructor, sets n = 0 in GField(2, 1)
	 */
	GFNumber();
	
	/**
	 * Constructor that receives a number, sets GField(2, 1)
	 * @param n
	 */
	GFNumber(long n);
	
	/**
	 * Copy constructor
	 * @param other GFNumber to copy
	 */
	GFNumber(const GFNumber &other);
	
	/**
	 * Destructor
	 */
	~GFNumber();
	
	/**
	 * Get n
	 * @return n
	 */
	long getNumber() const;
	
	/**
	 * Get field of GFNumber
	 * @return field
	 */
	GField getField() const;
	
	/**
	 * Returns a dynamic array of numbers prime factors
	 * @param size will store the size of the array
	 * @return pointer to array of prime factors
	 */
	GFNumber* getPrimeFactors(int* size) const;
	
	/**
	 * Print calculated prime factors
	 */
	void printFactors() const;
	
	/**
	 * Check if number is prime
	 * @return true if prime, otherwise false
	 */
	bool getIsPrime() const;
	
	/**
	 * Set this GFNumber's values os other's values
	 * @param other other GFNumber
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator=(GFNumber const &other);
	
	/**
	 * Set this GFNumber's values as other's values
	 * @param other long number
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator=(long const &other);
	
	/**
	 * Add two GFNumbers
	 * @param other right GFNumber
	 * @return reference to adjusted GFNumber
	 */
	GFNumber operator+(GFNumber const &other);
	
	/**
	 * Add long to GFNumber
	 * @param other long parameter
	 * @return reference to adjusted GFNumber
	 */
	GFNumber operator+(long const &other);
	
	/**
	 * Add other to this GFNumber
	 * @param other right GFNumber
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator+=(GFNumber const &other);
	
	/**
	 * Add long to this GFNumber
	 * @param other long parameter
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator+=(long const &other);
	
	/**
	 * Subtract right GFNumber from left GFNumber
	 * @param other right GFNumber
	 * @return reference to adjusted GFNumber
	 */
	GFNumber operator-(GFNumber const &other);
	
	/**
	 * Subtract long from left GFNumber
	 * @param other long parameter
	 * @return reference to adjusted GFNumber
	 */
	GFNumber operator-(long const &other);
	
	/**
	 * Subtract right GFNumber from this GFNumber
	 * @param other right GFNumber
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator-=(GFNumber const &other);
	
	/**
	 * Subtract long from this GFNumber
	 * @param other long parameter
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator-=(long const &other);
	
	/**
	 * Multiply two GFNumbers
	 * @param other right GFNumber
	 * @return reference to adjusted GFNumber
	 */
	GFNumber operator*(GFNumber const &other);
	
	/**
	 * Multiply GFNumber by long
	 * @param other long parameter
	 * @return reference to adjusted GFNumber
	 */
	GFNumber operator*(long const &other);
	
	/**
	 * Multiply this GFNumber by other GFNumber
	 * @param other right GFNumber
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator*=(GFNumber const &other);
	
	/**
	 * Multiply this GFNumber by long
	 * @param other long parameter
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator*=(long const &other);
	
	/**
	 * Calculate modulo of left GFNumber over right GFNumber
	 * @param other right GFNumber
	 * @return reference to adjusted GFNumber
	 */
	GFNumber operator%(GFNumber const &other);
	
	/**
	 * Calculate modulo of left GFNumber over long
	 * @param other long parameter
	 * @return reference to adjusted GFNumber
	 */
	GFNumber operator%(long const &other);
	
	/**
	 * Set left GFNumber to be modulo using right GFNumber
	 * @param other right GFNumber
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator%=(GFNumber const &other);
	
	/**
	 * Set left GFNumber to be modulo using long
	 * @param other long parameter
	 * @return reference to adjusted GFNumber
	 */
	GFNumber& operator%=(long const &other);
	
	/**
	 * Compares between GFNumbers by n and order of field
	 * @param other
	 * @return true if above are same, false otherwise
	 */
	bool operator==(GFNumber const &other) const;
	
	/**
	 * Compares between GFNumber and a long
	 * @param other long type number
	 * @return true if GFNumber's num == other
	 */
	bool operator==(long const &other) const;
	
	/**
	 * Opposite of == operator
	 * @param other
	 * @return opposite of == result
	 */
	bool operator!=(GFNumber const &other) const;
	
	/**
	 * Opposite of != comparison with long
	 * @param other long number
	 * @return opposite of ==
	 */
	bool operator!=(long const &other) const;
	
	/**
	 * Compares two GFNumbers by n (crashes if field order doesn't match)
	 * @param other
	 * @return true if left is bigger, false otherwise
	 */
	bool operator>(GFNumber const &other) const;
	
	/**
	 * Compares GFNumber to long
	 * @param other long num
	 * @return true if GFNumber is larger, otherwise false
	 */
	bool operator>(long const &other) const;
	
	/**
	 * Compares two GFNumbers by n (crashes if field order doesn't match)
	 * @param other
	 * @return true if left is bigger or equal, false otherwise
	 */
	bool operator>=(GFNumber const &other) const;
	
	/**
	 * Compares GFNumber to long
	 * @param other long number
	 * @return true if GFNumber is larger or equal, otherwise false
	 */
	bool operator>=(long const &other) const;
	
	/**
	 * Opposite of >=
	 * @param other
	 * @return true if right is bigger, false otherwise
	 */
	bool operator<(GFNumber const &other) const;
	
	/**
	 * Oppoisite of >= comparison with long
	 * @param other long number
	 * @return opposite of >=
	 */
	bool operator<(long const &other) const;
	
	/**
	 * Opposite of >
	 * @param other
	 * @return true if right is bigger or equal, false otherwise
	 */
	bool operator<=(GFNumber const &other) const;
	
	/**
	 * Opposite of > comparison with long
	 * @param other long number
	 * @return opposite of >
	 */
	bool operator<=(long const &other) const;
	
	/**
	 * Create output for printing
	 * @param out stores output
	 * @param number GFNumber object to print
	 * @return the result
	 */
	friend std::ostream& operator<<(std::ostream& out, GFNumber const &number);
	
	/**
	 * Read data into GFNumber object
	 * @param in data
	 * @param number GFNumber object
	 * @return data
	 */
	friend std::istream& operator>>(std::istream& in, GFNumber &number);
};

#endif
