#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include "Fractal.h"

#define VALID_ARG_AMT 2
#define DELIMITER ','

/**
 * Check if letter is either '\n', '\t' or '\r'
 * @param letter
 * @return
 */
bool isNTR(char letter)
{
	return (letter == '\n' || letter == '\t' || letter == '\r');
}

/**
 * Check validity of line in file
 * @return true if valid, otherwise false
 */
bool isValidLine(std::string &line, std::stack<Fractal> &fractals)
{
	for(char letter: line)
	{
		if((letter > '9' || letter < '0') && letter != DELIMITER && !isNTR(letter))
		{
			return false;
		}
	}
	
	std::istringstream temp(line);
	
	std::string value;
	int counter = 0;
	
	int fractalID = 0;
	int fractalDim = 0;
	
	while(std::getline(temp, value, DELIMITER))
	{
		int tempI;
		double tempD;
		
		try
		{
			tempI = std::stoi(value);
			tempD = std::stod(value);
		}
		catch(std::invalid_argument &e)
		{
			return false;
		}
		
		/* Check that input isn't a double */
		if(tempI != tempD)
		{
			return false;
		}
		
		if(counter == 0)
		{
			if(tempI > 3 || tempI < 1)
			{
				return false;
			}
			
			fractalID = tempI;
		}
		else if(counter == 1)
		{
			if(tempI > 6 || tempI < 1)
			{
				return false;
			}
			
			fractalDim = tempI;
		}
		
		if(counter > 1)
		{
			return false;
		}
		
		counter++;
	}

	if(counter != 2)
	{
		return false;
	}
	
	fractals.push(Fractal::fractalFactory(fractalID, fractalDim));
	
	return true;
}

/**
 * Check validity of supplied file path
 * @param fileName
 * @return true if valid, otherwise false
 */
bool isValidFile(const std::string &fileName, std::stack<Fractal> &fractals)
{
	std::fstream file;
	file.open(fileName);
	
	if(!file)
	{
		file.close();
		return false;
	}
	
	std::string line;
	while(std::getline(file, line))
	{
		if(!isValidLine(line, fractals))
		{
			file.close();
			return false;
		}
	}
	
	file.close();
	return true;
}

/**
 * Checks validity of supplied arguments
 * @param argc amount of args
 * @param argv arguments
 * @return EXIT_SUCCESS if valid, otherwise EXIT_FAILURE
 */
int areValidArgs(int argc, char** argv, std::stack<Fractal> &fractals)
{
	if(argc != VALID_ARG_AMT)
	{
		std::cerr << "Usage: FractalDrawer <file path>" << std::endl;
		return EXIT_FAILURE;
	}
	
	if(!isValidFile(argv[1], fractals))
	{
		std::cerr << "Invalid input" << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

/**
 * Main function for running program
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv)
{
	std::stack<Fractal> fractals;
	
	if(areValidArgs(argc, argv, fractals) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}
	
	while(!fractals.empty())
	{
		std::cout << fractals.top() << std::endl;
		fractals.pop();
	}

	return EXIT_SUCCESS;
}