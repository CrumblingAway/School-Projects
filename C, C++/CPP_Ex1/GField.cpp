#include <cmath>
#include <cassert>
#include "GField.h"
#include "GFNumber.h"

#define DEF_BASE 2
#define DEF_DEGREE 1

/**
 * Constructor that receives base and degree
 * @param p base
 * @param l degree
 */
GField::GField(const long &p, const long &l)
{
	long val = p;
	
	if(p < 0)
	{
		val *= -1;
	}
	this -> base = val;
	this -> degree = l;
}

/**
 * Default constructor, sets base = 2 and degree = 1
 */
GField::GField(): GField(DEF_BASE, DEF_DEGREE) {}

/**
 * Constructor that receives base, sets degree = 1
 * @param p base
 */
GField::GField(const long &p): GField(p, DEF_DEGREE) {}

/**
 * Copy constructor
 * @param other GField object to copy
 */
GField::GField(const GField &other): GField(other.base, other.degree) {};

/**
 * Destructor
 */
GField::~GField()
= default;

/**
 * Get char (base) of field
 * @return
 */
long GField::getChar() const
{
	return this -> base;
}

/**
 * Get degree of field
 * @return
 */
long GField::getDegree() const
{
	return this -> degree;
}

/**
 * Get order of field (base to the power of the degree)
 * @return
 */
long GField::getOrder() const
{
	assert(GField::isPrime(this -> base) && this -> degree > 0);
	return (long) pow(this -> base, this -> degree);
}

/**
 * Find greatest common divisor of a and b
 * @param a
 * @param b
 * @return gcd of a and b
 */
GFNumber GField::gcd(GFNumber a, GFNumber b)
{
	/* GFNumber representing zero */
	if(b.getNumber() == 0)
	{
		return a;
	}
	
	return gcd(b, a % b);
}

/**
 * Get adjusted instance of k in field
 * @param k
 * @return
 */
GFNumber GField::createNumber(long k) const
{
	long order = this -> getOrder();
	k %= order;
	
	return GFNumber(k, *this);
}

/**
 * Static method for checking if num is prime
 * @param num number to check
 * @return true if prime, otherwise false
 */
bool GField::isPrime(long num)
{
	if(num <= 1)
	{
		return false;
	}
	for(int i = 2; i <= (int)floor(sqrt(num)); i++)
	{
		if(num % i == 0)
		{
			return false;
		}
	}
	
	return true;
}

/**
 * Set this GField's values as other's values
 * @param other other GField
 * @return reference to adjusted GField
 */
GField& GField::operator=(GField const &other)
{
	this -> base = other.base;
	this -> degree = other.degree;
	return *this;
}

/**
 * Compares between GFields by order
 * @param other
 * @return true if order is same, otherwise false
 */
bool GField::operator==(GField const &other) const
{
	return (this -> getOrder() == other.getOrder());
}

/**
 * Opposite of == operator
 * @param other
 * @return opposite of == result
 */
bool GField::operator!=(GField const &other) const
{
	return !(*this == other);
}

/**
 * Create output for printing
 * @param out stores output
 * @param field GField object to print
 * @return the result
 */
std::ostream& operator<<(std::ostream& out, GField const &field)
{
	out << "GF(" << field.base << "**" << field.degree << ")";
	return out;
}

/**
 * Read data into GField object
 * @param in data
 * @param field GField object
 * @return data
 */
std::istream& operator>>(std::istream& in, GField &field)
{
	in >> field.base;
	assert(!in.fail());
	in >> field.degree;
	assert(!in.fail());
	return in;
}
