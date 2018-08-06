/* astrometry.h : misc astrometry/trigonometry utilities (domsa@konkoly.hu) */
/* for the Hungarian Automatic Telescope Project */

#ifndef ASTROMETRY_H__
#define ASTROMETRY_H__


/** \file astrometry.h
    \brief general astrometry routines and definitions

    Astronomical coordinates, conversions, etc.
*/

#include "utils.h"
#include "point.h"



typedef struct skypos
{
    real RA;
    real Dec;
} skypos;

typedef struct projpos
{
    real xi;
    real eta;
} projpos;

typedef struct pixpos
{
    real x;
    real y;
} pixpos;


/* exported functions */

/* arc projection (input output in radians) */
void  astr_arc(real /*ra*/, real /*dec*/, real /*rac*/, real /*decc*/,
	       /*returned: */ real * /*xi*/, real * /*eta*/);

/* arc projection (input output in degrees) */
void  astr_rarc(real /*ra*/, real /*dec*/, real /*rac*/, real /*decc*/,
		/*returned: */ real * /*xi*/, real * /*eta*/);

void  astr_rarc_vec(
        const point* input,
        real rac, real decc,
        size_t num_elems,
        point* output
);

/* inverse arc projection (input output in radians) */
void  astr_iarc(real /*xi*/, real /*eta*/, real /*rac*/, real /*decc*/,
		/*returned: */ real * /*ra*/, real * /*dec*/);

/* inverse arc projection (input output in degrees) */
void  astr_irarc(real /*xi*/, real /*eta*/, real /*rac*/, real /*decc*/,
		 /*returned: */ real * /*ra*/, real * /*dec*/);

/* gnomonic projection (radian) */
void    astr_gnomonic(
            real ra, real dec,      /* input sky coordinates */
            real rac, real decc,    /* input central coordinates */
            real* xi, real* eta     /* output projected coordinates */
        );

/* inverse gnomonic projection (radian) */
void    astr_ignomonic(
            real xi, real eta,      /* input projected coordinates */
            real rac, real decc,    /* input central coordinates */
            real* ra, real* dec     /* output sky coordinates*/
        );

/* gnomonic projection (degree) */
void    astr_rgnomonic(
            real ra, real dec,      /* input sky coordinates */
            real rac, real decc,    /* input central coordinates */
            real* xi, real* eta     /* output projected coordinates */
        );

/* inverse gnomonic projection (degree) */
void    astr_rignomonic(
            real xi, real eta,      /* input projected coordinates */
            real rac, real decc,    /* input central coordinates */
            real* ra, real* dec     /* output sky coordinates*/
        );

/* gnomonic projection (degree) for point list */
void    astr_rgnomonic_vec(
            const point* input,     /* input sky coordinates (vector) */
            size_t num_elems,       /* number of elements in input vector */
            real rac, real decc,    /* input central coordinates */
            point* output           /* output projected coordinates */
        );


/* projection of spherical coords into tangent plane (gnomonic projection) */
void  astr_s2tp(real /*ra*/, real /*dec*/, real /*rac*/, real /*decc*/, 
                /*returned: */ real * /*xi*/, real * /*eta*/);

/* projection of spherical coords into tangent plane (input/output in degs) */
void  astr_rs2tp(real /*ra*/, real /*dec*/, real /*rac*/, real /*decc*/, 
                /*returned: */ real * /*xi*/, real * /*eta*/);

/* transform tangent plane coords into spherical (input/output in radians) */
void  astr_tp2s(real /*xi*/, real /*eta*/, real /*rac*/, real /*decc*/,
		/*returned: */ real * /*ra*/, real * /*dec*/);

/* transform tangent plane coords into spherical (input/output in degrees) */
void  astr_rtp2s(real /*xi*/, real /*eta*/, real /*rac*/, real /*decc*/,
		/*returned: */ real * /*ra*/, real * /*dec*/);

/* project spherical coords into direction cosines */
void  astr_sp2dc(real /*ra*/, real /*dec*/, real * /*dv[3]*/);

/* separation angle between to points */
real  astr_sep(real /*ra1*/, real /*dec1*/, real /*ra2*/, real /*dec2*/);

/* separation angle between to points in degrees */
real  astr_rsep(real /*ra1*/, real /*dec1*/, real /*ra2*/, real /*dec2*/);

/* calc the position angle of one point relative to another (IO in rads) */
real  astr_bear(real /*ra1*/, real /*dec1*/, real /*ra2*/, real /*dec2*/);

/* calc the position angle of one point relative to another (IO in degs) */
real  astr_rbear(real /*ra1*/, real /*dec1*/, real /*ra2*/, real /*dec2*/);

/* normalize angle into range 0 - 2PI */
real  astr_ranorm(real /*angle*/);


/** converts equatorial coordinates into horizontal ones

    The function takes a star hour angle (in hours: between -12 and
    +12 hour), its declination (in degrees: between -90 and +90 deg) and
    the observers latitude (in degrees: between -90 and +90), and returns
    the stars azimuth, hour angle, zenith distance and airmass.
    The airmass is calculated using a formulae in Kitchin's photometry book.
    Return FALSE on error.

    \param hourangle  hour angle in degrees (between -180 and 180 degs)
    \param delta      declination in degrees (between -90 and 90 degs)
    \param latitude   latitude in degrees (between -90 and 90 degs)
    \param azimuth    returned: azimuth (increasing from S towards W) in
    degrees (between -180 and 180)
    \param zenithdist  returned: zenith distance in degrees (between 0 and
    180 degs)
    \param airmass  returned: airmass at the given position (between 1 and Inf)
*/
bool    astr_eq2hour(
            real hourangle, real delta, real latitude,
            real *azimuth, real *zenithdist, real *airmass
        );

/** calculates the hour angle from the local sideral time and from the RA

    returns the hour angle in hours specified by the local sideral time
    (the hour angle of the vernal equinox: between -12 and 12 hours)

    \param  lst  local sideral time in hours
    \param  ra  RA in hours
*/
real    astr_hourangle( real lst, real ra );

/** calculates distance and position angle between two points

    input and output in degrees, posangle can be NULL
*/
real astr_distance( real RA1, real Dec1, real RA2, real Dec2, real* posangle );
real astr_skydistance( const skypos* pos1, const skypos* pos2, real* posangle );

/** calculates distance and position angle between two points

    input and output in radians, posangle can be NULL
*/
real astr_rdistance( real RA1, real Dec1, real RA2, real Dec2, real* posangle );

#endif /* ASTROMETRY_H__ */
