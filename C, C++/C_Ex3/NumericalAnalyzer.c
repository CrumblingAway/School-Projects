#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "infi.h"

#define VALID_ARG_NUM 7

/**
 * Verify that str represents a valid double value
 * @param str string we're trying to parse
 * @param size length of str as an array
 * @return EXIT_FAILURE if invalid double, otherwise EXIT_SUCCESS
 */
int isValidDouble(const char *str, unsigned int size)
{
	unsigned int decCounter = 0;
	unsigned int decIdx = 0;
	for(unsigned int i = 0; i < size; i++)
	{
		if(i == 0 && str[i] == '-')
		{
			continue;
		}

		if(str[i] == '.')
		{
			decIdx = i;
			decCounter++;
			if(decCounter > 1)
			{
				return EXIT_FAILURE;
			}
		}

		if(isdigit(str[i]) == 0 && str[i] != '.')
		{
			return EXIT_FAILURE;
		}
	}

	if(decIdx == 1 && isdigit(str[0]) == 0)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/**
 * Check if number represented by str points to a valid function (i.e. between 1 and 6)
 * @param str string representing function number
 * @param size length of str array
 * @return EXIT_FAILURE if doesn't represent function, EXIT_SUCCESS otherwise
 */
int isValidNum(const char *str, unsigned int size)
{
	for(unsigned int i = 0; i < size; i++)
	{
		if(isdigit(str[i]) == 0)
		{
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/**
 * Verify that arguments are valid, i.e. argv[0] points to a valid function,
 * argv[1-2] represent the segment, etc.
 * @param argv arguments
 * @param size length of array argv
 * @return EXIT_FAILURE if invalid, otherwise EXIT_SUCCESS
 */
int areValidArgs(char **argv, int size)
{
	for(int i = 1; i < size; i++)
	{
		/* First input: number of function (between 1 and 6) */
		if(i == 1)
		{
			if(isValidNum(argv[i], (short) strlen(argv[i])) == EXIT_FAILURE)
			{
				return EXIT_FAILURE;
			}

			char *temp;
			int funcNum = (short) strtol(argv[i], &temp, 10);
			if(funcNum < 1 || funcNum > 6)
			{
				return EXIT_FAILURE;
			}
		}
			/* Second input: start and end of segment, second argument must be smaller */
		else if(i == 2)
		{
			if(isValidDouble(argv[i], (short) strlen(argv[i])) == EXIT_FAILURE ||
			   isValidDouble(argv[i + 1], (short) strlen(argv[i + 1])) == EXIT_FAILURE)
			{
				return EXIT_FAILURE;
			}

			char *ptr1;
			char *ptr2;
			double d1 = strtod(argv[i], &ptr1);
			double d2 = strtod(argv[i + 1], &ptr2);
			if(d1 > d2)
			{
				return EXIT_FAILURE;
			}
		}
			/* Already handled, continue to next i */
		else if(i == 3)
		{
			continue;
		}
			/* Fourth input: number of segments, integer greater than 0 */
		else if(i == 4)
		{
			if(isValidNum(argv[i], (short) strlen(argv[i])) == EXIT_FAILURE)
			{
				return EXIT_FAILURE;
			}

			char *temp;
			int partNum = (short) strtol(argv[i], &temp, 10);
			if(partNum < 1)
			{
				return EXIT_FAILURE;
			}
		}
			/* Fifth input: point at which we calculate the derivative */
		else if(i == 5)
		{
			if(isValidDouble(argv[i], (short) strlen(argv[i])) == EXIT_FAILURE)
			{
				return EXIT_FAILURE;
			}
		}
			/* Sixth input: epsilon for calculating derivative, must be greater than 0 */
		else if(i == 6)
		{
			if(isValidDouble(argv[i], (short) strlen(argv[i])) == EXIT_FAILURE)
			{
				return EXIT_FAILURE;
			}

			char *ptr;
			double epsilon = strtod(argv[i], &ptr);
			if(epsilon <= 0.0)
			{
				return EXIT_FAILURE;
			}
		}
		else
		{
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/**
 * Makes sure format of input is valid (not the input itself)
 * @param argc
 * @param argv
 * @return EXIT_FAILURE if format is invalid, otherwise EXIT_SUCCESS
 */
int isValidInput(int argc, char **argv)
{
	/* Amount of args has to be VALID_ARG_NUM (by default 7, including name of file) */
	if(argc != VALID_ARG_NUM)
	{
		fprintf(stderr, "Usage: NumericalAnalyzer <function number> <a> <b> <N> <x0> <h>\n");
		return EXIT_FAILURE;
	}

	if(areValidArgs(argv, argc) == EXIT_FAILURE)
	{
		fprintf(stderr, "Invalid input\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/**
 * f1 from exercise
 * @param x
 * @return f1(x)
 */
double func1(double x)
{
	return (pow(x, 2) / 4);
}

/**
 * f2 from exercise
 * @param x
 * @return f2(x)
 */
double func2(double x)
{
	double numerator = (-1) * pow(x, 3) + 3 * pow(x, 2) + x - 4 * sqrt(x);
	double denominator = 2 * x * sqrt(x);
	return numerator / denominator;
}

/**
 * f3 from exercise
 * @param x
 * @return f3(x)
 */
double func3(double x)
{
	return pow(sin(x), 2) - pow(cos(x), 2);
}

/**
 * f4 from exercise
 * @param x
 * @return f4(x)
 */
double func4(double x)
{
	return sin(x) / (1 + cos(2 * x));
}

/**
 * f5 from exercise
 * @param x
 * @return f5(x)
 */
double func5(double x)
{
	return pow(M_E, x);
}

/**
 * f6 from exercise
 * @param x
 * @return f6(x)
 */
double func6(double x)
{
	return sinh(2 * x) / (M_E * pow(x, 2));
}

/**
 * Factory that returns a pointer to a RealFunction
 * @param i index of function
 * @return pointer to RealFunction
 */
RealFunction functionFactory(int i)
{
	switch(i)
	{
		case 1:
			return &func1;
		case 2:
			return &func2;
		case 3:
			return &func3;
		case 4:
			return &func4;
		case 5:
			return &func5;
		case 6:
			return &func6;
		default:
			return NULL;
	}
}

/**
 * Parses inputs from cmd, assumes input has already been verified
 * @param argv arguments
 * @param pI pointer to graph index
 * @param pStart pointer to start of segment
 * @param pEnd pointer to end of segment
 * @param pParts pointer to amount of parts in segment
 * @param pPoint pointer to point for derivative
 * @param pEpsilon ponter to epsilon for derivative
 */
void parseInputs(char **argv, int *pI, double *pStart, double *pEnd, unsigned int *pParts, double *pPoint, double *pEpsilon)
{
	char *temp;
	*pI = (int) strtol(argv[1], &temp, 10);
	*pStart = strtod(argv[2], &temp);
	*pEnd = strtod(argv[3], &temp);
	*pParts = (unsigned int) strtol(argv[4], &temp, 10);
	*pPoint = strtod(argv[5], &temp);
	*pEpsilon = strtod(argv[6], &temp);
}

/**
 * Main function for running program
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
	/* Check if input format is valid */
	if(isValidInput(argc, argv) == EXIT_FAILURE)
	{
		exit(EXIT_FAILURE);
	}

	/* Save input */
	int i = 0; double start = 0; double end = 0; unsigned int parts = 0; double point = 0; double epsilon = 0;
	parseInputs(argv, &i, &start, &end, &parts, &point, &epsilon);
	RealFunction myFunc = functionFactory(i);

	printf("Integral: %.3f\n", integration(myFunc, start, end, parts));
	printf("Derivative: %.f\n", derivative(myFunc, point, epsilon));
}