#include <iostream>
#include <cassert>
#include "GField.h"
#include "GFNumber.h"

/**
 * Main function
 * @return
 */
int main()
{
	GFNumber num1, num2;
	
	std::cin >> num1 >> num2;
	
	std::cout << num1 + num2 << std::endl;
	std::cout << num1 - num2 << std::endl;
	std::cout << num2 - num1 << std::endl;
	std::cout << num1 * num2 << std::endl;
	num1.printFactors();
	num2.printFactors();
	
	return 0;
}
