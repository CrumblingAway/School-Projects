#include <iostream>
#include <cmath>
#include <cassert>
#include "GFNumber.h"

#define DEF_NUM 0
#define DEF_ARR_SIZE 10
#define F(x) (x*x + 1)
#define RESIZE_FACTOR 2

/**
 * Adjusts negative numbers to given range in modulo
 * @param num
 * @param range
 * @return adjusted num
 */
long negativeModulo(long num, long range)
{
	while(num < 0)
	{
		num += range;
	}
	
	return num;
}


/**
 * Constructor that receives a number and a field
 */
GFNumber::GFNumber(long n, GField newField)
{
	this -> field = newField;
	n = negativeModulo(n, this -> field.getOrder());
	n %= this -> field.getOrder();
	this -> num = n;
}

/**
 * Default constructor, sets n = 0 in GField(2, 1)
 */
GFNumber::GFNumber(): GFNumber(DEF_NUM, GField()) {}

/**
 * Constructor that receives a number, sets GField(2, 1)
 * @param n
 */
GFNumber::GFNumber(long n): GFNumber(negativeModulo(n, GField().getOrder()) % GField().getOrder(), GField()) {}

/**
 * Copy constructor
 * @param other GFNumber to copy
 */
GFNumber::GFNumber(const GFNumber &other): GFNumber(other.num, other.field) {}

/**
 * Destructor
 */
GFNumber::~GFNumber()
= default;

/**
 * Get n
 * @return n
 */
long GFNumber::getNumber() const
{
	return this -> num;
}

/**
 * Get field of GFNumber
 * @return field
 */
GField GFNumber::getField() const
{
	return this -> field;
}

/**
 * Performs Pollard's Rho algorithm on a numbber
 * @param n positive odd integer
 * @return one of n's prime divisors
 */
long getRho(GFNumber n)
{
	/* Num has to be odd */
	if(n % 2 == 0)
	{
		return -1;
	}
	
	GFNumber x = n.getField().createNumber((long)(rand() % (n.getNumber()) + 1));
	GFNumber y = x;
	long p = 1;
	
	while(p == 1)
	{
		x = F(x);
		y = F(F(y));
		p = n.getField().gcd(x - y, n).getNumber();
	}
	
	if(n == p)
	{
		return -1;
	}
	
	return p;
}

/**
 * Resize given array by resize factor
 * @param arr array to resize
 * @param size amount of occupied cells in array (from the left)
 * @param arrSize size of array
 */
void resize(GFNumber **arr, int const *size, int &arrSize)
{
	arrSize *= RESIZE_FACTOR;
	auto *newArr = new GFNumber[arrSize];
	for(int i = 0; i < *size; i++)
	{
		newArr[i] = (*arr)[i];
	}
	
	delete[] *arr;
	*arr = newArr;
}

/**
 * Returns a dynamic array of numbers prime factors
 * @param size will store the size of the array
 * @return pointer to array of prime factors
 */
GFNumber* GFNumber::getPrimeFactors(int* size) const
{
	*size = 0;
	
	GFNumber *result = new GFNumber[DEF_ARR_SIZE];
	int arrSize = DEF_ARR_SIZE;
	
	GFNumber copyNum = *this;
	
	while(copyNum > 1)
	{
		/* Perform Rho algorithm until it fails */
		long temp = getRho(copyNum);
		while(temp != -1 && GField::isPrime(temp))
		{
			/* Resize array */
			if(*size == DEF_ARR_SIZE)
			{
				resize(&result, size, arrSize);
			}
			
			result[*size] = this -> field.createNumber(temp);
			(*size)++;
			copyNum = copyNum.field.createNumber(copyNum.num / temp);
			temp = getRho(copyNum);
		}
		
		/* Brute force division after Rho failed */
		for(long divisor = 2; divisor <= (long) floor(sqrt(copyNum.getNumber())); divisor++)
		{
			/* Resize array */
			if(*size == DEF_ARR_SIZE)
			{
				resize(&result, size, arrSize);
			}
			
			if(copyNum % divisor == 0)
			{
				result[*size] = this -> field.createNumber(divisor);
				(*size)++;
				copyNum = copyNum.field.createNumber(copyNum.num / divisor);
				
				break;
			}
		}
		
		/* If number is prime then we add the number itself and 1 */
		if(copyNum.getIsPrime())
		{
			if(*size == 0)
			{
				result[*size] = copyNum.field.createNumber(copyNum.getNumber());
				return result;
			}
			
			/* Resize array */
			if(*size == DEF_ARR_SIZE)
			{
				resize(&result, size, arrSize);
			}
			
			result[*size] = copyNum.field.createNumber(copyNum.getNumber());
			(*size)++;
			copyNum = 1;
		}
	}
	
	return result;
}

/**
 * Print calculated prime factors
 */
void GFNumber::printFactors() const
{
	int size = 0;
	GFNumber *result = getPrimeFactors(&size);
	
	std::cout << this -> num << "=";
	
	/* Edges cases: num = 0 or 1 (size == 0)
	 * or num is prime (size == 1) */
	if(size == 0 || size == 1)
	{
		std::cout << this -> num << "*" << "1" << std::endl;
		delete[] result;
		return;
	}
	
	/* Print all factors */
	for(int i = 0; i < size - 1; i++)
	{
		std::cout << result[i].getNumber() << "*";
	}
	std::cout << result[size - 1].getNumber() << std::endl;
	
	delete[] result;
}

/**
 * Check if number is prime
 * @return true if prime, otherwise false
 */
bool GFNumber::getIsPrime() const
{
	return GField::isPrime(this -> num);
}

/**
 * Set this GFNumber's values os other's values
 * @param other other GFNumber
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator=(GFNumber const &other)
{
	this -> num = other.getNumber();
	this -> field = other.getField();
	
	return *this;
}

/**
 * Set this GFNumber's values as other's values
 * @param other long number
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator=(long const &other)
{
	long newVal = other;
	newVal = negativeModulo(newVal, this -> field.getOrder());
	this -> num = newVal;
	
	return *this;
}

/**
 * Add two GFNumbers
 * @param other right GFNumber
 * @return reference to adjusted GFNumber
 */
GFNumber GFNumber::operator+(GFNumber const &other)
{
	assert(this -> field.getOrder() == other.field.getOrder());
	
	GFNumber output;
	
	long result = (this -> num + other.getNumber()) % (this -> field.getOrder());
	result = negativeModulo(result, this -> field.getOrder());
	output = result;
	output.field = this -> field;
	return output;
}

/**
 * Add long to GFNumber
 * @param other long parameter
 * @return reference to adjusted GFNumber
 */
GFNumber GFNumber::operator+(long const &other)
{
	GFNumber output;
	
	long result = (this -> num + other) % (this -> field.getOrder());
	result = negativeModulo(result, this -> field.getOrder());
	output = result;
	output.field = this -> field;
	return output;
}

/**
 * Add other to this GFNumber
 * @param other right GFNumber
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator+=(GFNumber const &other)
{
	*this = *this + other;
	return *this;
}

/**
 * Add long to this GFNumber
 * @param other long parameter
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator+=(long const &other)
{
	*this = *this + other;
	return *this;
}

/**
 * Subtract right GFNumber from left GFNumber
 * @param other right GFNumber
 * @return reference to adjusted GFNumber
 */
GFNumber GFNumber::operator-(GFNumber const &other)
{
	assert(this -> field.getOrder() == other.field.getOrder());
	
	GFNumber output;
	
	long result = (this -> num - other.getNumber()) % (this -> field.getOrder());
	result = negativeModulo(result, this -> field.getOrder());
	output = result;
	output.field = this -> field;
	return output;
}

/**
 * Subtract long from left GFNumber
 * @param other long parameter
 * @return reference to adjusted GFNumber
 */
GFNumber GFNumber::operator-(long const &other)
{
	GFNumber output;
	
	long result = (this -> num - other) % (this -> field.getOrder());
	negativeModulo(result, this -> field.getOrder());
	output = result;
	output.field = this -> field;
	return output;
}

/**
 * Subtract right GFNumber from this GFNumber
 * @param other right GFNumber
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator-=(GFNumber const &other)
{
	*this = *this - other;
	return *this;
}

/**
 * Subtract long from this GFNumber
 * @param other long parameter
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator-=(long const &other)
{
	*this = *this - other;
	return *this;
}

/**
 * Multiply two GFNumbers
 * @param other right GFNumber
 * @return reference to adjusted GFNumber
 */
GFNumber GFNumber::operator*(GFNumber const &other)
{
	assert(this -> field.getOrder() == other.field.getOrder());
	
	GFNumber output;
	
	long result = (this -> num * other.getNumber()) % (this -> field.getOrder());
	result = negativeModulo(result, this -> field.getOrder());
	output = result;
	output.field = this -> field;
	return output;
}

/**
 * Multiply GFNumber by long
 * @param other long parameter
 * @return reference to adjusted GFNumber
 */
GFNumber GFNumber::operator*(long const &other)
{
	GFNumber output;
	
	long result = (this -> num * other) % (this -> field.getOrder());
	result = negativeModulo(result, this -> field.getOrder());
	output = result;
	output.field = this -> field;
	return output;
}

/**
 * Multiply this GFNumber by other GFNumber
 * @param other right GFNumber
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator*=(GFNumber const &other)
{
	*this = *this * other;
	return *this;
}

/**
 * Multiply this GFNumber by long
 * @param other long parameter
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator*=(long const &other)
{
	*this = *this * other;
	return *this;
}

/**
 * Calculate modulo of left GFNumber over right GFNumber
 * @param other right GFNumber
 * @return reference to adjusted GFNumber
 */
GFNumber GFNumber::operator%(GFNumber const &other)
{
	assert(this -> field.getOrder() == other.field.getOrder());
	
	GFNumber output;
	
	long result = (this -> num) % (other.getNumber());
	negativeModulo(result, this -> field.getOrder());
	output = result;
	output.field = this -> field;
	return output;
}

/**
 * Calculate modulo of left GFNumber over long
 * @param other long parameter
 * @return reference to adjusted GFNumber
 */
GFNumber GFNumber::operator%(long const &other)
{
	GFNumber output;
	
	long val = other;
	val = negativeModulo(val, this -> field.getOrder());
	
	long result = (this -> num) % val;
	result = negativeModulo(result, this -> field.getOrder());
	output = result;
	output.field = this -> field;
	return output;
}

/**
 * Set left GFNumber to be modulo using right GFNumber
 * @param other right GFNumber
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator%=(GFNumber const &other)
{
	*this = *this % other;
	return *this;
}

/**
 * Set left GFNumber to be modulo using long
 * @param other long parameter
 * @return reference to adjusted GFNumber
 */
GFNumber& GFNumber::operator%=(long const &other)
{
	*this = *this % other;
	return *this;
}

/**
 * Compares between GFNumbers by n and order of field
 * @param other
 * @return true if above are same, false otherwise
 */
bool GFNumber::operator==(GFNumber const &other) const
{
	return this -> num == other.getNumber() && this->field.getOrder() == other.getField().getOrder();
}

/**
 * Compares between GFNumber and a long
 * @param other long type number
 * @return true if GFNumber's num == other
 */
bool GFNumber::operator==(long const &other) const
{
	return this -> num == other;
}

/**
 * Opposite of == operator
 * @param other
 * @return opposite of == result
 */
bool GFNumber::operator!=(GFNumber const &other) const
{
	return !(*this == other);
}

/**
 * Opposite of != comparison with long
 * @param other long number
 * @return opposite of ==
 */
bool GFNumber::operator!=(long const &other) const
{
	return this -> num == other;
}

/**
 * Compares two GFNumbers by n (crashes if field order doesn't match)
 * @param other
 * @return true if left is bigger, false otherwise
 */
bool GFNumber::operator>(GFNumber const &other) const
{
	assert(this -> field.getOrder() == other.field.getOrder());
	
	return this -> num > other.getNumber();
	
}

/**
 * Compares GFNumber to long
 * @param other long num
 * @return true if GFNumber is larger, otherwise false
 */
bool GFNumber::operator>(long const &other) const
{
	return this -> num > other;
	
}

/**
 * Compares two GFNumbers by n (crashes if field order doesn't match)
 * @param other
 * @return true if left is bigger or equal, false otherwise
 */
bool GFNumber::operator>=(GFNumber const &other) const
{
	assert(this -> field.getOrder() == other.field.getOrder());
	
	return this -> num >= other.getNumber();
}

/**
 * Compares GFNumber to long
 * @param other long number
 * @return true if GFNumber is larger or equal, otherwise false
 */
bool GFNumber::operator>=(long const &other) const
{
	return this->num >= other;
	
}

/**
 * Opposite of >=
 * @param other
 * @return true if right is bigger, false otherwise
 */
bool GFNumber::operator<(GFNumber const &other) const
{
	return !(*this >= other);
}

/**
 * Oppoisite of >= comparison with long
 * @param other long number
 * @return opposite of >=
 */
bool GFNumber::operator<(long const &other) const
{
	return this -> num < other;
}

/**
 * Opposite of >
 * @param other
 * @return true if right is bigger or equal, false otherwise
 */
bool GFNumber::operator<=(GFNumber const &other) const
{
	return !(*this > other);
}

/**
 * Opposite of > comparison with long
 * @param other long number
 * @return opposite of >
 */
bool GFNumber::operator<=(long const &other) const
{
	return this -> num <= other;
}

/**
 * Create output for printing
 * @param out stores output
 * @param number GFNumber object to print
 * @return the result
 */
std::ostream& operator<<(std::ostream& out, GFNumber const &number)
{
	out << number.num << " " << number.field;
	return out;
}

/**
 * Read data into GFNumber object
 * @param in data
 * @param number GFNumber object
 * @return data
 */
std::istream& operator>>(std::istream& in, GFNumber &number)
{
	in >> number.num;
	assert(!in.fail());
	in >> number.field;
	assert(!in.fail());
	number.num = negativeModulo(number.num, number.field.getOrder());
	number.num %= number.field.getOrder();
	return in;
}

