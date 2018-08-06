
#include "pmotion.h"
#include "astromath.h"
#include "mmath.h"

#include <math.h>


/* ========================================================================= */
/* calculates proper motion mu_ra*cos Dec, mu_dec [mas/year] */
void pmotion(
    const skypos*   pos1,       /* RA, Dec [deg] */
    const skypos*   pos2,       /* RA, Dec [deg] */
    real            tdiff,      /* time difference [Julian Years] */
    real*           pm_ra,      /* mu_ra*cos Dec [mas/year] */
    real*           pm_dec      /* mu_dec [mas/year] */
)
{
    if ( IS_ZERO( tdiff ) ) {
        *pm_ra = 0.0;
        *pm_dec = 0.0;
        return;
    }

    *pm_ra =    DEG2MAS( ( pos2->RA - pos1->RA ) )
            *   cos( DEG2RAD( pos1->Dec ) )
            /   tdiff;
    *pm_dec = DEG2MAS( pos2->Dec - pos1->Dec ) / tdiff;
}

/* ========================================================================= */
/* calculates total proper motion and position angle */
void pmotion_total(
    const skypos*   pos1,       /* RA, Dec [deg] */
    const skypos*   pos2,       /* RA, Dec [deg] */
    real            tdiff,      /* time difference [Julian Years] */
    real*           pm_tot,     /* total proper motion [mas/year] */
    real*           pos_angle   /* position angle [deg] */
)
{
    *pm_tot = astr_rdistance(
        pos1->RA, pos1->Dec,
        pos2->RA, pos2->Dec,
        pos_angle   // out
    );

    *pm_tot = DEG2MAS( *pm_tot );
    *pm_tot /= tdiff;
}

/* ========================================================================= */

/* calculates total proper motion and position angle from proper motion */
void pmotion2total(
    real            pm_ra,
    real            pm_dec,
    real*           pm_tot,
    real*           pm_angle
)
{
    *pm_tot = mdist( pm_ra, pm_dec );
    *pm_angle = mposangle( pm_ra, pm_dec );
}

/* ========================================================================= */
void pmotion_apply(
    double          *ra,        /* [deg] in/out */
    double          *dec,       /* [deg] in/out */
    real            pm_ra,      /* mu_ra*cos Dec [mas/yr] */
    real            pm_dec,     /* mu_dec [mas/yr] */
    real            tdiff       /* time difference [yr] */
)
{
    real Dec = *dec + MAS2DEG( pm_dec * tdiff );

    if ( !IS_EQUAL( ABS( *dec ), 90 ) ) {
        *ra += MAS2DEG( pm_ra * tdiff / cos( DEG2RAD( *dec ) ) );
        *dec = Dec;
    }
    else if ( !IS_EQUAL( ABS( Dec ), 90 ) ) {
        *dec = Dec;
        *ra += MAS2DEG( pm_ra * tdiff / cos( DEG2RAD( *dec ) ) );
    }

    *ra = dmodRA( *ra );
    *dec = dmodDec( *dec );
}

void pmotion_apply_coordpm(
    double*         ra,        /* [deg] in/out */
    double*         dec,
    real            pm_ra,      /* coordinate proper motion [ms/yr] */
    real            pm_dec,     /* mu_dec [mas/yr] */
    real            tdiff       /* time difference [yr] */
)
{
    *ra += HOUR2DEG( MAS2DEG( pm_ra * tdiff ) );
    *dec += MAS2DEG( pm_dec * tdiff );

    *ra = dmodRA( *ra );
    *dec = dmodDec( *dec );
}

/* ========================================================================= */

void pmotion_precess(
    skypos*         pos,        /* in/out */
    real            JD0,        /* starting epoch [Julian days] */
    real            JD,         /* final epoch [Julian days] */
    real            pm_ra,      /* mu_ra*cos Dec [mas/yr] */
    real            pm_dec,     /* mu_dec [mas/yr] */
    bool            use_ang_pm  /* angular proper motion or coordinate pm */
)
{
    // based on J. Meeus: Astronomical Algorithms

    real T = ( JD0 - EPOCH_J2000 ) / JULIAN_CENTURY;
    real t = ( JD - JD0 ) / JULIAN_CENTURY;

    // coefficients are in arc seconds
    real lincorr = ( 2306.2181 + 1.39659 * T - 0.000139 * T * T ) * t;

    real xi =   lincorr                                                 // t
            +   ( 0.30188 - 0.000344 * T ) * t * t                      // t^2
            +   0.017988 * t * t * t;                                   // t^3

    real zeta = lincorr                                                 // t
            +   ( 1.09468 + 0.000066 * T ) * t * t                      // t^2
            +   0.018203 * t * t * t;                                   // t^3

    real theta = ( 2004.3109 - 0.85330 * T - 0.000217 * T * T ) * t     // t
            -   ( 0.42665 + 0.000217 * T ) * t * t                      // t^2
            -   0.041833 * t * t * t;                                   // t^3

    xi = DEG2RAD( ARCSEC2DEG( xi ) );
    zeta = DEG2RAD( ARCSEC2DEG( zeta ) );
    theta = DEG2RAD( ARCSEC2DEG( theta ) );

    // proper motion needs to be calculated first
    if ( use_ang_pm ) {
        pmotion_apply( &pos->RA,&pos->Dec, pm_ra, pm_dec, ( JD - JD0 ) / JYEAR_IN_DAYS );
    }
    else {
        pmotion_apply_coordpm(
            &pos->RA,&pos->Dec, pm_ra, pm_dec, ( JD - JD0 ) / JYEAR_IN_DAYS
        );
    }

    // calculate precession

    real RA = DEG2RAD( pos->RA );
    real Dec = DEG2RAD( pos->Dec );

    real sinDec = sin( Dec );
    real cosDec = cos( Dec );
    real cosDecRAxi = cosDec * cos( RA + xi );
    real sinTheta = sin( theta );
    real cosTheta = cos( theta );

    real A = cosDec * sin( RA + xi );
    real B = cosTheta * cosDecRAxi - sinTheta * sinDec;
    real C = sinTheta * cosDecRAxi + cosTheta * sinDec;

    RA = atan2( A, B ) + zeta;
    if ( PI_2 - ABS( Dec ) > ASTR_SMALL_ANGLE_RAD ) {
        Dec = asin( C );
    }
    else {
        Dec = acos( sqrt( A * A + B * B ) );
    }

    pos->RA = dmodRA( RAD2DEG( RA ) );
    pos->Dec = RAD2DEG( Dec );
}

void pmotion_precess2000(
    double*         ra,        /* in/out */
    double*         dec,
    real            JD,         /* final epoch [actually years] */
    real            pm_ra,      /* mu_ra*cos Dec [mas/yr] */
    real            pm_dec,     /* mu_dec [mas/yr] */
    bool            use_ang_pm  /* angular proper motion or coordinate pm */
)
{
    // based on J. Meeus: Astronomical Algorithms

    real t = ( JD - 2000 ) / 100;

    // coefficients are in arc seconds
    real lincorr = 2306.2181 * t;

    real xi =   lincorr + 0.30188 * t * t + 0.017998 * t * t * t;
    real zeta = lincorr + 1.09468 * t * t + 0.018203 * t * t * t;
    real theta = 2004.3109 * t - 0.42665 * t * t  - 0.041833 * t * t * t;

    xi = DEG2RAD( ARCSEC2DEG( xi ) );
    zeta = DEG2RAD( ARCSEC2DEG( zeta ) );
    theta = DEG2RAD( ARCSEC2DEG( theta ) );

    // proper motion needs to be calculated first
    if ( use_ang_pm ) {
        pmotion_apply(ra,dec, pm_ra, pm_dec, JD-2000);
    }
    else {
        pmotion_apply_coordpm(ra,dec, pm_ra, pm_dec,JD - 2000);
    }
    // calculate precession

    real RA = DEG2RAD( *ra );
    real Dec = DEG2RAD( *dec );

    real sinDec = sin( Dec );
    real cosDec = cos( Dec );
    real cosDecRAxi = cosDec * cos( RA + xi );
    real sinTheta = sin( theta );
    real cosTheta = cos( theta );

    real A = cosDec * sin( RA + xi );
    real B = cosTheta * cosDecRAxi - sinTheta * sinDec;
    real C = sinTheta * cosDecRAxi + cosTheta * sinDec;

    RA = atan2( A, B ) + zeta;
    if ( PI_2 - ABS( Dec ) > ASTR_SMALL_ANGLE_RAD ) {
        Dec = asin( C );
    }
    else {
        Dec = acos( sqrt( A * A + B * B ) );
    }

    *ra = dmodRA( RAD2DEG( RA ) );
    *dec = RAD2DEG( Dec );
}

/* ========================================================================= */
