/* astrometry.c : misc astrometry/trigonometry utilities (domsa@konkoly.hu) */
/* for the Hungarian Automatic Telescope Project */

#include "astrometry.h"
#include "mmath.h"
#include "astromath.h"

#include <math.h>
#include <string.h>


/* arc projection (input output in radians) */
void  astr_arc(real ra, real dec, real rac, real decc, real *xi, real *eta)
{
    real  cdecc = cos(decc);
    real  sdecc = sin(decc);
    real  crac  = cos(rac);
    real  srac  = sin(rac);
    real  uu    = cos(dec)*cos(ra);
    real  vv    = cos(dec)*sin(ra);
    real  ww    = sin(dec);
    real  uun   = uu*cdecc*crac + vv*cdecc*srac + ww*sdecc;
    real  vvn   = -uu*srac + vv*crac;
    real  wwn   = -uu*sdecc*crac - vv*sdecc*srac + ww*cdecc;
    real  denom = sqrt(vvn*vvn + wwn*wwn);
    real  auun  = acos(uun);

    if ( !IS_ZERO( denom ) ) {
        *xi  = auun * vvn / denom;
        *eta = auun * wwn/ denom;
    }
    else {
        *xi = 0;
        *eta = 0;
    }
}  /* endof astr_arc() */


/* arc projection (input output in degrees) */
void  astr_rarc(real ra, real dec, real rac, real decc, real *xi, real *eta)
{
  real  rra = DEG2RAD(ra);
  real  rdec = DEG2RAD(dec);
  real  rrac = DEG2RAD(rac);
  real  rdecc = DEG2RAD(decc);

  astr_arc(rra, rdec, rrac, rdecc, xi, eta);

  *xi  = RAD2DEG(*xi);
  *eta = RAD2DEG(*eta);
}  /* endof astr_rarc() */

void  astr_rarc_vec(
        const point* input,
        real rac, real decc,
        size_t num_elems,
        point* output
)
{
    rac = DEG2RAD( rac );
    decc = DEG2RAD( decc );

    real cdecc = cos( decc );
    real sdecc = sin( decc );
    real crac  = cos( rac );
    real srac  = sin( rac );
    real cdecccrac = cdecc * crac;
    real cdeccsrac = cdecc * srac;

    for ( size_t idx = 0; idx < num_elems; idx++ ) {
        real ra = DEG2RAD( input[idx].xcord );
        real dec = DEG2RAD( input[idx].ycord );

        real uu    = cos( dec ) * cos( ra );
        real vv    = cos( dec ) * sin( ra );
        real ww    = sin( dec );
        real uun   = uu * cdecccrac + vv * cdeccsrac + ww * sdecc;
        real vvn   = vv * crac - uu * srac;
        real wwn   = ww * cdecc - uu * sdecc * crac - vv * sdecc * srac;
        real denom = sqrt( vvn * vvn + wwn * wwn );
        real auun  = acos( uun );

        if ( !IS_ZERO( denom ) ) {
            output[idx].xcord = RAD2DEG( auun * vvn / denom );
            output[idx].ycord = RAD2DEG( auun * wwn/ denom );
        }
        else {
            output[idx].xcord = 0.0;
            output[idx].ycord = 0.0;
        }
    }
}


/* inverse arc projection (input output in radians) */
void  astr_iarc(real xi, real eta, real rac, real decc, real *ra, real *dec)
{
  real  dist  = sqrt(xi*xi + eta*eta);

  if(dist < REAL_EPSILON)
  {
    *ra  = rac;
    *dec = decc;
  }
  else
  {
    real  uu    = cos(dist);
    real  vv    = xi *sin(dist)/dist;
    real  ww    = eta*sin(dist)/dist;

    real  cdecc = cos(decc);
    real  sdecc = sin(decc);
    real  crac  = cos(rac);
    real  srac  = sin(rac);

    real  uun   = uu*cdecc*crac - vv*srac - ww*sdecc*crac;
    real  vvn   = uu*cdecc*srac + vv*crac - ww*sdecc*srac;
    real  wwn   = uu*sdecc + ww*cdecc;

    *dec = asin(wwn);

    if ( IS_ZERO( uun ) ) {
      *ra = SIGN(xi)*PI_2;
    }
    else
    {
      *ra = atan(vvn/uun);

      if(ABS(decc + eta) > PI_2)
        *ra += 2.0*(PI - ABS(*ra - rac));

      real cosdec = cos( *dec );
      if (  !IS_ZERO( cosdec )
        &&  rac + xi / cosdec > PI_2
        &&  rac + xi / cosdec < PI3_2
      ) {
        *ra += PI;
      }

      if ( *ra < 0.0 )
        *ra += PI2;
      else if( *ra > PI2 )
        *ra -= PI2;
    }
  }
}  /* endof astr_iarc() */


/* inverse arc projection (input output in degrees) */
void  astr_irarc(real xi, real eta, real rac, real decc, real *ra, real *dec)
{
  real  rxi   = DEG2RAD(xi);
  real  reta  = DEG2RAD(eta);
  real  rrac  = DEG2RAD(rac);
  real  rdecc = DEG2RAD(decc);

  astr_iarc(rxi, reta, rrac, rdecc, ra, dec);

  *ra  = RAD2DEG(*ra);
  *dec = RAD2DEG(*dec);
}  /* endof astr_irarc() */


/* gnomonic projection (radian) */
void astr_gnomonic(
    real ra, real dec,      /* input sky coordinates */
    real rac, real decc,    /* input central coordinates */
    real* xi, real* eta     /* output projected coords */
)
{
    real dra = ra - rac;
    real cosdra = cos( dra );
    real cosdec = cos( dec );
    real cosdecc = cos( decc );
    real sindec = sin( dec );
    real sindecc = sin( decc );
    real cosc = sindecc * sindec + cosdecc * cosdec * cosdra;

    if ( IS_ZERO( cosc ) ) {
        *xi = 0.0;      // TODO!!!
        *eta = 0.0;
    }
    else {
        *xi = ( cosdec * sin( dra ) ) / cosc;
        *eta = ( cosdecc * sindec - sindecc * cosdec * cosdra ) / cosc;
    }
}

/* inverse gnomonic projection (radian) */
void astr_ignomonic(
    real xi, real eta,      /* input projected coords */
    real rac, real decc,    /* input central coordinates */
    real* ra, real* dec     /* output sky coordinates*/
)
{
    real rho = sqrt( xi * xi + eta * eta );
    real c = atan( rho );
    real sinc = sin( c );
    real cosc = cos( c );
    real sindecc = sin( decc );
    real cosdecc = cos( decc );

    *ra = rac +
        atan2( xi * sinc, rho * cosdecc * cosc - eta * sindecc * sinc );
    *dec = asin( cosc * sindecc + eta * sinc * cosdecc / rho );
}

/* gnomonic projection (degree) */
void astr_rgnomonic(
    real ra, real dec,      /* input longitude, latitude */
    real rac, real decc,    /* input central coordinates */
    real* xi, real* eta     /* output */
)
{
    real  rra   = DEG2RAD( ra );
    real  rdec  = DEG2RAD( dec );
    real  rrac  = DEG2RAD( rac );
    real  rdecc = DEG2RAD( decc );

    astr_gnomonic( rra, rdec, rrac, rdecc, xi, eta );

    *xi  = RAD2DEG( *xi );
    *eta = RAD2DEG( *eta );
}

/* inverse gnomonic projection (degree) */
void astr_rignomonic(
    real xi, real eta,      /* input projected coords */
    real rac, real decc,    /* input central coordinates */
    real* ra, real* dec     /* output sky coordinates*/
)
{
    real rxi   = DEG2RAD( xi );
    real reta  = DEG2RAD( eta );
    real rrac  = DEG2RAD( rac );
    real rdecc = DEG2RAD( decc );

    astr_ignomonic( rxi, reta, rrac, rdecc, ra, dec );

    *ra  = dmodRA( RAD2DEG( *ra ) );
    *dec = dmodDec( RAD2DEG( *dec ) );
}

/* gnomonic projection (degree) for point list */
void astr_rgnomonic_vec(
    const point* input,
    size_t num_elems,
    real rac, real decc,
    point* output
)
{
    rac = DEG2RAD( rac );
    decc = DEG2RAD( decc );
    real cosdecc = cos( decc );
    real sindecc = sin( decc );

    for ( size_t idx = 0; idx < num_elems; idx++ ) {
        real ra = DEG2RAD( input[idx].xcord );
        real dec = DEG2RAD( input[idx].ycord );
        real dra = ra - rac;
        real cosdra = cos( dra );
        real sindec = sin( dec );
        real cosdec = cos( dec );
        real cosc = sindecc * sindec + cosdecc * cosdec * cosdra;

        if ( IS_ZERO( cosc ) ) {
            output[idx].xcord = 0.0;    // TODO!!!
            output[idx].ycord = 0.0;
        }
        else {
            real xi = ( cosdec * sin( dra ) ) / cosc;
            real eta = ( cosdecc * sindec - sindecc * cosdec * cosdra ) / cosc;
            output[idx].xcord = RAD2DEG( xi );
            output[idx].ycord = RAD2DEG( eta );
        }
    }
}

/* projection of spherical coords into tangent plane (gnomonic projection) */
void  astr_s2tp(real ra, real dec, real rac, real decc, real *xi, real *eta)
{
  real  sdecc  = sin(decc);
  real  sdec   = sin(dec);
  real  cdecc  = cos(decc);
  real  cdec   = cos(dec);
  real  radif  = ra - rac;
  real  sradif = sin(radif);
  real  cradif = cos(radif);
  real  denom  = sdec*sdecc + cdec*cdecc*cradif;

  *xi  = cdec*sradif/denom;     // TODO!!! check denom
  *eta = (sdec*cdecc - cdec*sdecc*cradif)/denom;
}  /* endof astr_s2tp() */

/* projection of spherical coords into tangent plane (input/output in degs) */
void  astr_rs2tp(real ra, real dec, real rac, real decc, real *xi, real *eta)
{
  real  rra    = DEG2RAD(ra);
  real  rdec   = DEG2RAD(dec);
  real  rrac   = DEG2RAD(rac);
  real  rdecc  = DEG2RAD(decc);
  real  sdecc  = sin(rdecc);
  real  sdec   = sin(rdec);
  real  cdecc  = cos(rdecc);
  real  cdec   = cos(rdec);
  real  radif  = rra - rrac;
  real  sradif = sin(radif);
  real  cradif = cos(radif);
  real  denom  = sdec*sdecc + cdec*cdecc*cradif;

  *xi  = RAD2DEG(cdec*sradif/denom);    // TODO!!! check denom
  *eta = RAD2DEG((sdec*cdecc - cdec*sdecc*cradif)/denom);
}  /* endof astr_rs2tp() */


/* transform tangent plane coords into spherical */
void  astr_tp2s(real xi, real eta, real rac, real decc, real *ra, real *dec)
{
  real  sdecc = sin(decc);
  real  cdecc = cos(decc);
  real  denom = cdecc - eta*sdecc;
  real  radif = atan2(xi, denom);

  *ra  = astr_ranorm(radif + rac);
  *dec = decc + atan2(sdecc + eta*cdecc, sqrt(xi*xi + denom*denom));
}  /* endof astr_tp2s() */


/* transform tangent plane coords into spherical (input/output in degs) */
void  astr_rtp2s(real xi, real eta, real rac, real decc, real *ra, real *dec)
{
  real  rxi   = DEG2RAD(xi);
  real  reta  = DEG2RAD(eta);
  real  rrac  = DEG2RAD(rac);
  real  rdecc = DEG2RAD(decc);

  real  sdecc = sin(rdecc);
  real  cdecc = cos(rdecc);
  real  denom = cdecc - reta*sdecc;
  real  radif = atan2(rxi, denom);

  *ra  = astr_ranorm(radif + rrac);
  *ra  = RAD2DEG(*ra);
  *dec = decc +
    RAD2DEG(atan2(sdecc + reta*cdecc, sqrt(rxi*rxi + denom*denom)));
}  /* endof astr_rtp2s() */


/* project spherical coords into direction cosines */
void  astr_sp2dc(real ra, real dec, real dv[3])
{
  real cosdec = cos( dec );

  dv[0] = cos( ra ) * cosdec;
  dv[1] = sin( ra ) * cosdec;
  dv[2] = sin( dec );
}  /* endof astr_sp2dc() */


/* separation angle between to points */
real  astr_sep(real ra1, real dec1, real ra2, real dec2)
{
  int   ii;
  real  dist, sep, v1[3], v2[3], c2;

  astr_sp2dc(ra1, dec1, v1);
  astr_sp2dc(ra2, dec2, v2);

  for(sep = 0.0, ii = 0; ii < 3; ii++)
  {
    dist = v1[ii] - v2[ii];
    sep += dist*dist;
  }
  sep /= 4.0;

  /* angle between the vectors */
  c2 = 1.0 - sep;

  return  2.0*atan2(sqrt(sep), sqrt(fabs( c2 )));
  //return  2.0*atan2(sqrt(sep), sqrt(MAX(0.0, c2)));
}  /* endof astr_sep() */


/* separation angle between to points in degrees */
real  astr_rsep(real ra1, real dec1, real ra2, real dec2)
{
  ra1  = DEG2RAD(ra1);
  dec1 = DEG2RAD(dec1);
  ra2  = DEG2RAD(ra2);
  dec2 = DEG2RAD(dec2);

  return  RAD2DEG(astr_sep(ra1, dec1, ra2, dec2));
}  /* endof astr_rsep() */


/* calc the position angle of one point relative to another (IO in rads) */
real  astr_bear(real ra1, real dec1, real ra2, real dec2)
{
  real  da, xx, yy;

  da = ra2 - ra1;

  yy = sin(da)*cos(dec2);
  xx = sin(dec2)*cos(dec1) - cos(dec2)*sin(dec1)*cos(da);

  if ( IS_ZERO( xx ) && IS_ZERO( yy ) ) {
    da = 0.0;
  }
  else {
    da = atan2(yy, xx);
  }

  return  da;
}  /* endof astr_bear() */


/* calc the position angle of one point relative to another (IO in degs) */
real  astr_rbear(real ra1, real dec1, real ra2, real dec2)
{
  ra1  = DEG2RAD(ra1);
  dec1 = DEG2RAD(dec1);
  ra2  = DEG2RAD(ra2);
  dec2 = DEG2RAD(dec2);

  return  RAD2DEG(astr_bear(ra1, dec1, ra2, dec2));
}  /* endof astr_rbear() */


/* normalize angle into range 0 - 2PI */
real  astr_ranorm(real angle)
{
  real  ang = dmod(angle, PI2);

  return  ang >= 0.0 ? ang : ang + PI2;
}  /* endof astr_ranorm() */


/* converts equatorial coordinates into horizontal ones */
bool astr_eq2hour(
    real ha, real delta, real phi,
    real* azimuth, real *zdist, real* airmass
)
{
  real  hdist;
  real  sinm, cosm;
  real  tmp;

  /* check input parameters */
  if(ha < -12 || ha > 12 || delta < -90 || delta > 90
     || phi < -90 || phi > 90)
    return  false;

  /* convert degrees to radian */
  ha    = DEG2RAD(HOUR2DEG(ha));
  delta = DEG2RAD(delta);
  phi   = DEG2RAD(phi);


  sinm = sin(phi)*sin(delta) + cos(phi)*cos(delta)*cos(ha);
  cosm = sqrt(1.0 - sinm*sinm);

  /* horizontal distance */
  hdist = asin(sinm);

  /* calculate azimuth */
  if ( IS_ZERO( cosm ) )
  {
    /* horizon distance is +-90 degree (zenith/nadir) */
    *azimuth = 0.0;
  }
  else
  {
    real  sinA = cos(delta)*sin(ha)/cosm;
    real  cosA = (-cos(phi)*sin(delta) + sin(phi)*cos(delta)*cos(ha))/cosm;

    if ( IS_ZERO( sinA ) )
    {
      if(cosA < 0.0)
        *azimuth = -PI;
      else
        *azimuth = 0.0;
    }
    else
    {
      real  tanAper2 = (1.0 - cosA)/sinA;
      *azimuth = 2*atan(tanAper2);
    }
  }

  /* calculate the air mass */
  tmp = PI_2 - hdist;
  tmp = 1/cos(tmp) - 1;

  /* Kitchin's photometry, p.87 */
  *airmass = ABS(tmp + 1) - 0.0018167*tmp - 0.002875*pow(tmp, 2.0)
    - 0.0008083*pow(tmp, 3.0);

  /* convert coordinates back to degrees */
  *azimuth = RAD2DEG(*azimuth);
  *zdist   = 90.0 - RAD2DEG(hdist);

  return  true;
}  /* endof astr_eq2hour() */


/* calculate the hour angle from the local sideral time and from the RA */
real astr_hourangle( real lst, real ra )
{
  real  ha = lst - ra;

  if(ha < -12.0)
    ha += 24.0;
  else if(ha > 12.0)
    ha -= 24.0;

  return  ha;
}  /* endof astr_hourangle() */

// Observing and measuring visual double stars (p. 203)
real astr_distance( real RA1, real Dec1, real RA2, real Dec2, real* posangle )
{
    real rdist =
        astr_rdistance(
            DEG2RAD( RA1 ), DEG2RAD( Dec1 ),
            DEG2RAD( RA2 ), DEG2RAD( Dec2 ),
            posangle
        );

    if ( posangle ) {
        *posangle = RAD2DEG( *posangle );
    }

    return RAD2DEG( rdist );
}

real astr_skydistance( const skypos* pos1, const skypos* pos2, real* posangle )
{
    return astr_distance( pos1->RA, pos1->Dec, pos2->RA, pos2->Dec, posangle );
}

real astr_rdistance( real RA1, real Dec1, real RA2, real Dec2, real* posangle )
{
    real dRA = RA2 - RA1;
    real dDec = Dec2 - Dec1;
    real dist = mdist( dRA * cos( ( Dec1 + Dec2 ) / 2 ), dDec );

    if ( dist > ASTR_SMALL_ANGLE_RAD ) {
        // re-calculate distance from the exact formulae
        dist = sin( Dec1 )*sin( Dec2 ) + cos( Dec1 )*cos( Dec2 )*cos( dRA );
        dist = acos( dist );
    }

    if ( posangle ) {
        *posangle = mposangler( dRA, dDec );
    }

    return dist;
}
