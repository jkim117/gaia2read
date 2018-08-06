
#ifndef ASTRO_MATH_H__
#define ASTRO_MATH_H__

#include "utils.h"

/* ========================================================================= */

#define DEG2HOUR(x)     ( (x) / 15.0 )
#define HOUR2DEG(x)     ( (x) * 15.0 )
#define MIN2DEC(x)      ( (x) / 60.0 )
#define DEC2MIN(x)      ( (x) * 60.0 )
#define SEC2DEC(x)      ( (x) / 3600.0 )
#define DEC2SEC(x)      ( (x) * 3600.0 )

#define DEG2ARCSEC(x)   DEC2SEC(x)
#define ARCSEC2DEG(x)   SEC2DEC(x)
#define ARCSEC2MIN(x)   MIN2DEC(x)
#define DEG2MAS(x)      (1000.0 * DEC2SEC(x))
#define MAS2DEG(x)      (ARCSEC2DEG(x) / 1000.0)

#define JYEAR_IN_DAYS   365.25      /* Julian year in days */
#define JULIAN_CENTURY  36525.0     /* Julian century in days */
#define EPOCH_J2000     2451545.0   /* [JD] J2000.0: 2000.01.01 12:00 TT */

#ifndef ASTR_SMALL_ANGLE_RAD
#define  ASTR_SMALL_ANGLE_RAD   0.003   /* ~11" */
#endif


/* ========================================================================= */

/* normalizes RA to 0 - 360.0 and Dec to -90 to +90 */
real    dmodRA( real RA );
real    dmodDec( real Dec );

/* ========================================================================= */

/* calculates modified Julian Date from calendar date and time */
real    modjuliandate(
            int year, int month, int day,
            int hour, int min, real sec
        );

/* calculates Julian Date from calendar date and time */
real    juliandate( 
            int year, int month, int day,
            int hour, int min, real sec
        );

/* converts modified Julian Date to calendar date plus hours */
void    mjd2caldatehr(
            real mjd,                           /* in: MJD */
            int* year, int* month, int* day,    /* out: date */
            real* hour                          /* out: time in hours */
        );

/* converts modified Julian Date to calendar date and time */
void    mjd2caldate(
            real mjd,                           /* in: MJD */
            int* year, int* month, int* day,    /* out: date */
            int* hour, int* min, real* sec      /* out: time */
        );

/* converts Julian Date to calendar date plus hours */
void    jd2caldatehr(
            real jd,                            /* in: JD */
            int* year, int* month, int* day,    /* out: date */
            real* hour                          /* out: time in hours */
        );

/* converts Julian Date to calendar date and time */
void    jd2caldate(
            real jd,                            /* in: JD */
            int* year, int* month, int* day,    /* out: date */
            int* hour, int* min, real* sec      /* out: time */
        );

/* converts Julian Epoch JYYYY.YY to JD */
real    epoch2jd( real epoch );

/* converts JD to Julian Epoch JYYYY.YY */
real    jd2epoch( real jd );

/* calculates modified Julian Date from Julian Date */
real    mjd2jd( real mjd );

/* calculates Julian Date from modified Julian Date */
real    jd2mjd( real jd );

/* ========================================================================= */

/* converts degrees, minutes and seconds to decimal representation */
real    deg2dec( int deg, int min, real sec );

/* converts degrees to sexagesimal representation (hh:mm:ss.ss) */
void    deg2HMS( real deg, int* hour, int* min, real* sec );

/* converts hours to sexagesimal representation (hh:mm:ss.ss.ss) */
void    hrs2HMS( real hrs, int* hour, int* min, real* sec );

/* converts degrees to sexagesimal representation (dd:mm:ss.ss) */
void    deg2DMS( real deg, int* dout, int* min, real* sec );

/* ========================================================================= */
#endif /*ASTRO_MATH_H__*/
