
#ifndef M_MATH_H__
#define M_MATH_H__

#include "utils.h"


/** small number near to floating-point zero */
#ifndef REAL_EPSILON
# ifndef SINGLEPREC
#  define REAL_EPSILON 1E-9
# else
#  define REAL_EPSILON 1E-6
# endif
#endif /*REAL_EPSILON*/

#ifndef REAL_MAX
# ifndef SINGLEPREC
#  define REAL_MAX      1E37
# else
#  define REAL_MAX      1E39
# endif
#endif

#ifndef REAL_MIN
# ifndef SINGLEPREC
#  define REAL_MIN      1E-37
# else
#  define REAL_MIN      1E-39
# endif
#endif

#ifndef IS_ZERO
# define IS_ZERO(x) ( ABS( x ) < REAL_EPSILON )
#endif
#ifndef IS_EQUAL
# define IS_EQUAL(x, y) IS_ZERO( (x) - (y) )
#endif
#ifndef IS_LT
# define IS_LT(x, y)    ( ( (x) + REAL_EPSILON ) < ( y ) )
#endif
#ifndef IS_NEGATIVE
# define IS_NEGATIVE(x) IS_LT(x, 0.0)
#endif


/** define the famous number if it is not here */
#ifndef PI
# define PI           3.14159265358979

/** 2*PI */
# define PI2          6.28318530717958

/** PI/2 */
# define PI_2         1.57079632679490

/** 1.5*PI */
# define PI3_2        4.71238898038469
#endif


/** return the bigger value from \a x and \a y.
    Do not give functions as arguments to this macro! */
#define MAX(x,y)      ((x) > (y) ? (x) : (y))

static inline real MAXF( real x, real y ) {
    real x_temp = x;
    real y_temp = y;
    return x_temp > y_temp ? x_temp : y_temp;
}

/** return the smaller value from \a x and \a y.
    Do not give functions as arguments to this macro! */
#define MIN(x,y)      ((x) > (y) ? (y) : (x))

static inline real MINF( real x, real y ) {
    real x_temp = x;
    real y_temp = y;
    return x_temp > y_temp ? y_temp : x_temp;
}

/** return the absolute value of \a x
    Do not give functions as arguments to this macro! */
#define ABS(x)        ((x) > 0 ? (x) : -(x))

/** get the sign of \a x */
#define SIGN(x)       ((x) > 0 ? 1 : -1)

/** round a number (int or floating point) to the nearest integer */
#define ROUND(x)      ((int) ((x) + ((x) > 0 ? 0.5 : -0.5)))

/* radian degree conversion */
#undef RAD2DEG
#define RAD2DEG(x)   ((x)*57.2957795131)
#undef DEG2RAD
#define DEG2RAD(x)   ((x)*0.01745329252)

/* ========================================================================= */

/* calculates floor of a number */
int     mfloor( real val );
long    mfloorl( real val );

/* calculates fractional part of a number */
real    mfrac( real val );

/* calculates num mod modulo */
real    dmod( real num, real modulo );
int     dmodi( real num, real modulo );

/* calculates base^exponent */
real    powri( real base, int exponent );

/* round number to decimal precision */
real    mround( real num, int precision );

/* calculates sqrt( x^2 + y^2 ) */
real    mdist( real xx, real yy );

/* calculates x^2 + y^2 (mdist ^ 2) */
real    mdist2( real xx, real yy );

/* calculates position angle [deg] of a vector, north is 0 clockwise to 360 */
real    mposangle( real xx, real yy );

/* calculates position angle [rad] of a vector, north is 0 clockwise to 2PI */
real    mposangler( real xx, real yy );

/* ========================================================================= */
#endif /*M_MATH_H__*/
