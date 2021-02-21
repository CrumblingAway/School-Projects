#include "infi.h"
#include <stdio.h>
#include <assert.h>

/**
 * Computes the numerical integration for the given rationale function
 * at the given numbers range, based on Riemann sums.
 * @return The integration result.
 */
double integration(RealFunction func, double start, double end, unsigned int parts)
{
	assert(start < end);
	assert(parts > 0);

	double result = 0.0;

	for(unsigned int i = 0; i < parts; i++)
	{
		/* Calculate midway point of segment */
		double segStart = start + i * (end - start) / parts;
		double segEnd = start + (i + 1) * (end - start) / parts;
		double mid = (segStart + segEnd) / 2;
		assert(!isnan(func(mid)));
		assert(!isinf(func(mid)));

		result += func(mid) * (end - start) / parts;
	}

	return result;
}

/**
 * Computes a numerical derivative for the given rationale function
 * at the point, for a given epsilon.
 * @return The differentiate.
 */
double derivative(RealFunction func, double point, double margin)
{
	assert(margin > 0);
	assert(!isnan(func(point + margin)));
	assert(!isnan(func(point - margin)));
	assert(!isinf(func(point + margin)));
	assert(!isinf(func(point - margin)));

	return ((func(point + margin) - func(point - margin)) / (2 * margin));
}
