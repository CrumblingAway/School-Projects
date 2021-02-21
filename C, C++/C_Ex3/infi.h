#ifndef LIBINFI_INFI_H
#   define LIBINFI_INFI_H
#   include <math.h>

/****************************************
 *  Register required constants, using thier POSIX values,
 *  in case they're not supplied by math.h in the given OS implementation.
 ****************************************/
#   ifndef M_E
#       define M_E         2.71828182845904523536028747135266250   /* e */
#   endif
#   ifndef M_PI
#       define M_PI        3.14159265358979323846264338327950288   /* pi */
#   endif

/* Define the anonymous function signature */
typedef double (*RealFunction)(double);

/****************************************
 *      API
 ****************************************/

/**
 * Computes the numerical integration for the given rationale function
 * at the given numbers range, based on Riemann sums.
 * @return The integration result.
 */
double integration(RealFunction, double, double, unsigned int);

/**
 * Computes a numerical derivative for the given rationale function
 * at the point, for a given epsilon.
 * @return The differentiate.
 */
double derivative(RealFunction, double, double);

#endif //LIBINFI_INFI_H