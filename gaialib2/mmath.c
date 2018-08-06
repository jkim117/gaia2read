
#include "mmath.h"

#include <math.h>


/* ========================================================================= */
/* calculates floor of a number */
int mfloor( real val )
{
    return ( int )mfloorl( val );
}

long mfloorl( real val )
{
    if ( IS_ZERO( val ) ) {
        return 0;
    }

    if ( val > 0 ) {
        return ( long )( val + REAL_EPSILON );
    }
    else {
        return ( long )( val - REAL_EPSILON );
    }
}

/* ========================================================================= */
/* calculates fractional part of a number */
real mfrac( real val )
{
    return val - mfloor( val );
}

/* ========================================================================= */
/* calculates num mod modulo */
real dmod( real num, real modulo )
{
    if ( IS_ZERO( num ) || IS_EQUAL( num, modulo ) || IS_ZERO( modulo ) )
        return 0;

    if ( num > 0 && num < modulo ) {
        return num;
    }

    while ( num < 0 ) {
        num += modulo;
    }

    return modulo * mfrac( num / modulo );
}

int dmodi( real num, real modulo )
{
    return mfloor( dmod( num, modulo ) );
}

/* ========================================================================= */
/* calculates sqrt( x^2 + y^2 ) */
real mdist( real xx, real yy )
{
    return hypot( xx, yy );
}

real mdist2( real xx, real yy )
{
    return xx * xx + yy * yy;
}

/* ========================================================================= */
/* calculates base^exponent */
real powri( real base, int exponent )
{
    real num = 1.0;
    while ( exponent > 0 ) {
        if ( exponent & 1 ) {
            num *= base;
            if ( exponent == 1 ) {
                return num;
            }
        }

        base *= base;
        exponent /= 2;
    }

    return num;
}

/* ========================================================================= */
/* round number to decimal precision */
real mround( real num, int precision )
{
    if ( !precision ) {
        return ceil( num - 0.5 );
    }

    real norm = powri( 10.0, precision );
    return ceil( num * norm - 0.5 ) / norm;
}

/* ========================================================================= */
/* calculates position angle [deg] of a vector, north is 0 */
real mposangle( real xx, real yy )
{
    return RAD2DEG( mposangler( xx, yy ) );
}

/* calculates position angle [rad] of a vector, north is 0 clockwise to 2PI */
real mposangler( real xx, real yy )
{
    real posangle = atan2( xx, yy );
    return dmod( posangle, PI2 );
}

/* ========================================================================= */
