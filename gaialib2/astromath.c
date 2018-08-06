
#include "astromath.h"
#include "mmath.h"

#include <string.h>
#include <math.h>

static const real AvgMonthInDays = 30.6001;

/* ========================================================================= */
/* normalizes RA to 0 - 360.0 and Dec to -90 to +90 */

real dmodRA( real RA )
{
    return dmod( RA, 360.0 );
}

/* ------------------------------------------------------------------------- */

real dmodDec( real Dec )
{
    if ( Dec > 90.0 ) {
        real delta = Dec - 90.0;
        return 90.0 - delta;
    }
    else if ( Dec < -90 ) {
        real delta = Dec + 90.0;
        return -90.0 - delta;
    }

    return Dec;
}

/* ========================================================================= */
/* calculates (modified) Julian Date from calendar date and time */

real modjuliandate( int year, int month, int day, int hour, int min, real sec )
{
    if ( month <= 2 ) {
        month += 12;
        year -= 1;
    }

    int b;
    if ( 10000L * year + 100L * month + day <= 15821004L ) {
        // Julian calendar
        b = ( year + 4716 )/4 - 1181;
    }
    else {
        // Gregorian calendar
        b = year/400 - year/100 + year/4;
    }

    long mjdmidnight = 365L * year - 679004L;
    mjdmidnight += b + mfloor( 30.6001 * ( month + 1 ) ) + day;
    return mjdmidnight + deg2dec( hour, min, sec ) / 24.0;
}

/* ------------------------------------------------------------------------- */

real juliandate( int year, int month, int day, int hour, int min, real sec )
{
    return mjd2jd ( modjuliandate( year, month, day, hour, min, sec ) );
}
/* ------------------------------------------------------------------------- */

void mjd2caldatehr(
    real mjd,                           /* in: MJD */
    int* year, int* month, int* day,    /* out: date */
    real* hour                          /* out: time in hours */
)
{
    long a, b, c, d, e, f;

    a = mfloorl( mjd + 2400001.0 );
    if ( a < 2299161 ) {
        // Julian calendar
        b = 0;
        c = a + 1524;
    }
    else {
        // Gregorian calendar
        b = mfloorl( ( a - 1867216.25)/ 36524.25 );
        c = a + b - b / 4 + 1525;
    }

    d = mfloorl( ( c - 122.1 ) / JYEAR_IN_DAYS );
    e = 365 * d + d / 4;
    f = mfloorl( ( c - e ) / AvgMonthInDays );

    *day = c - e - mfloor( AvgMonthInDays * f );
    *month = f - 1 - 12 * ( f / 14 );
    *year = d - 4715 - ( ( 7 + *month ) / 10 );

    *hour = 24.0 * mfrac( mjd );
}

/* ------------------------------------------------------------------------- */

void mjd2caldate(
    real mjd,                           /* in: MJD */
    int* year, int* month, int* day,    /* out: date */
    int* hour, int* min, real* sec      /* out: time */
)
{
    real hours;
    mjd2caldatehr( mjd, year, month, day, &hours );
    hrs2HMS( hours, hour, min, sec );
}

/* ------------------------------------------------------------------------- */

void jd2caldatehr(
    real jd,                            /* in: JD */
    int* year, int* month, int* day,    /* out: date */
    real* hour                          /* out: time in hours */
)
{
    mjd2caldatehr( jd2mjd( jd ), year, month, day, hour );
}

/* ------------------------------------------------------------------------- */

void jd2caldate(
    real jd,                            /* in: JD */
    int* year, int* month, int* day,    /* out: date */
    int* hour, int* min, real* sec      /* out: time */
)
{
    mjd2caldate( jd2mjd( jd ), year, month, day, hour, min, sec );
}

/* ------------------------------------------------------------------------- */

real epoch2jd( real epoch )
{
    return 2451545.0 + ( epoch - 2000.0 ) * JYEAR_IN_DAYS;
}

real jd2epoch( real jd )
{
    return ( jd - 2451545.0 ) / JYEAR_IN_DAYS + 2000.0;
}

/* ------------------------------------------------------------------------- */

real mjd2jd( real mjd )
{
    return mjd + 2400000.5;
}

real jd2mjd( real jd )
{
    return jd - 2400000.5;
}

/* ========================================================================= */
/* converts degrees, minutes and seconds to decimal representation */

real deg2dec( int deg, int min, real sec )
{
    int sign = deg < 0 || min < 0 || IS_NEGATIVE( sec ) ? -1 : 1;
    return
        sign * (
                fabs( deg )
            +   MIN2DEC( fabs( min ) )
            +   SEC2DEC( fabs ( sec ) )
        );
}

/* ========================================================================= */

/* converts hours to sexagesimal representation (hh:mm:ss.ss) */
void hrs2HMS( real hrs, int* hour, int* min, real* sec )
{
    hrs = dmod( hrs, 24.0 );

    *hour = mfloor( hrs );
    hrs = ( hrs - *hour ) * 60.0;
    *min = mfloor( hrs );
    *sec = ( hrs - *min ) * 60.0;
}

/* ------------------------------------------------------------------------- */

void deg2HMS( real deg, int* hour, int* min, real* sec )
{
    deg = dmodRA( deg );
    deg = DEG2HOUR( deg );

    *hour = mfloor( deg );
    deg = ( deg - *hour ) * 60.0;
    *min = mfloor( deg );
    *sec = ( deg - *min ) * 60.0;
}

/* ========================================================================= */
/* converts degrees to sextesimal representation (dd:mm:ss) */

void deg2DMS( real deg, int* dout, int* min, real* sec )
{
    bool is_negative = IS_NEGATIVE( deg );

    if ( IS_ZERO( deg ) ) {
        *dout = 0;
        *min = 0;
        *sec = 0.0;
        return;
    }

    deg = fabs( deg );
    deg = dmodDec( deg );

    *dout = mfloor( deg );
    deg = ( deg - *dout ) * 60.0;
    *min = mfloor( deg );
    *sec = ( deg - *min ) * 60.0;

    if ( is_negative ) {
        if ( *dout ) {
            *dout *= -1;
        }
        else if ( *min ) {
            *min *= -1;
        }
        else if ( !IS_ZERO( *sec ) ) {
            *sec *= -1.0;
        }
    }
}

/* ========================================================================= */
