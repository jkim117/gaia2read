/* point.c : operation on 2D points and arrays (domsa@konkoly.hu (2002)) */

#include "point.h"
#include "astrometry.h"
#include "mmath.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


/* create/set/destroy a point array */
pointarr  *point_createarr(void)
{
  pointarr  *arr = malloc(sizeof(*arr));

  if(arr != NULL)
  {
    arr->length = 0;
    arr->points = NULL;
  }

  return  arr;
}  /* endof point_createarr() */


pointarr  *point_buildarr(int length, point *points)
{
  pointarr  *arr = malloc(sizeof(*arr));

  if(arr != NULL)
  {
    arr->length = length;
    arr->points = points;
  }

  return  arr;
}  /* endof point_buildarr() */


void  point_destroyarr(pointarr *pa)
{
  if(pa != NULL)
  {
    free(pa->points);
    free(pa);
  }

  return;
}  /* endof point_destroyarr() */



/* calculate eucledian distance between two 2D points */
real  point_eucdist( const point* p1, const point* p2)
{
  real  xx, yy;

  xx = p1->xcord - p2->xcord;
  yy = p1->ycord - p2->ycord;

  return  sqrt(xx*xx + yy*yy);
}  /* endof point_eucdist() */


/* calculate squared eucledian distance between two 2D points */
real  point_eucdist2( const point* p1, const point* p2 )
{
  real  xx, yy;

  xx = p1->xcord - p2->xcord;
  yy = p1->ycord - p2->ycord;

  return  xx*xx + yy*yy;
}  /* endof point_eucdist() */

real  point_abs( const point* pt )
{
    real xx = pt->xcord;
    real yy = pt->ycord;

    return sqrt( xx * xx + yy * yy );
}

/* return minimum in x coordinates or 0 */
real  point_minx(int nelem, point *points)
{
  int   ii;
  real  minx;

  if(nelem < 1 || points == NULL)
    return  0;

  minx = points[0].xcord;

  for(ii = 1; ii < nelem; ii++)
        if(points[ii].xcord < minx)
      minx = points[ii].xcord;

  return  minx;
}  /* endof point_minx() */

/* return minimum in y coordinates or 0 */
real  point_miny(int nelem, point *points)
{
  int   ii;
  real  miny;

  if(nelem < 1 || points == NULL)
    return  0;

  miny = points[0].ycord;

  for(ii = 1; ii < nelem; ii++)
        if(points[ii].ycord < miny)
      miny = points[ii].ycord;

  return  miny;
}  /* endof point_miny() */

/* return maximum in x coordinates or 0 */
real  point_maxx(int nelem, point *points)
{
  int   ii;
  real  maxx;

  if(nelem < 1 || points == NULL)
    return  0;

  maxx = points[0].xcord;

  for(ii = 1; ii < nelem; ii++)
    if(points[ii].xcord > maxx)
      maxx = points[ii].xcord;

  return  maxx;
}  /* endof point_maxx() */

/* return maximum in y coordinates or 0 */
real  point_maxy(int nelem, point *points)
{
  int   ii;
  real  maxy;

  if(nelem < 1 || points == NULL)
    return  0;

  maxy = points[0].ycord;

  for(ii = 1; ii < nelem; ii++)
    if(points[ii].ycord > maxy)
      maxy = points[ii].ycord;

  return  maxy;
}  /* endof point_maxy() */


/* return the minimum and maximum coordinates along the x-axis */
void  point_minmaxx(int nelem, point *points, real *minx, real *maxx)
{
  int  ii;

  *minx = points->xcord;
  *maxx = points->xcord;

  for(ii = 0; ii < nelem; ii++)
    if(*minx > points[ii].xcord)
      *minx = points[ii].xcord;
    else if(*maxx < points[ii].xcord)
      *maxx = points[ii].xcord;

  return;
}  /* endof point_minmaxx() */


/* return the minimum and maximum coordinates along the y-axis */
void  point_minmaxy(int nelem, point *points, real *miny, real *maxy)
{
  int  ii;

  *miny = points->ycord;
  *maxy = points->ycord;

  for(ii = 0; ii < nelem; ii++)
    if(*miny > points[ii].ycord)
      *miny = points[ii].ycord;
    else if(*maxy < points[ii].ycord)
      *maxy = points[ii].ycord;

  return;
}  /* endof point_minmaxy() */


/* get length of x side */
real  point_lengthx(int nelem, point *points)
{
  real  minx, maxx;

  point_minmaxx(nelem, points, &minx, &maxx);

  return  maxx - minx;
}  /* endof point_lengthx() */


/* get length of y side */
real  point_lengthy(int nelem, point *points)
{
  real  miny, maxy;

  point_minmaxy(nelem, points, &miny, &maxy);

  return  maxy - miny;
}  /* endof point_lengthy() */


/* flip along x axis */
void  point_flipx(int nelem, point *points)
{
  int   ii;
  real  minx, maxx, midx;

  point_minmaxx(nelem, points, &minx, &maxx);
  midx = (minx + maxx)/2.0;

  for(ii = 0; ii < nelem; ii++)
    points[ii].xcord = 2.0*midx - points[ii].xcord;

  return;
}  /* endof point_flipx() */


/* flip along y axis */
void  point_flipy(int nelem, point *points)
{
  int   ii;
  real  miny, maxy, midy;

  point_minmaxy(nelem, points, &miny, &maxy);
  midy = (miny + maxy)/2.0;

  for(ii = 0; ii < nelem; ii++)
    points[ii].ycord = 2.0*midy - points[ii].ycord;

  return;
}  /* endof point_flipy() */


/* zoom in x axis */
void  point_magx(int nelem, point *points, real xmag)
{
  int  ii;

  for(ii = 0; ii < nelem; ii++)
    points[ii].xcord *= xmag;

  return;
}  /* endof point_magx() */

/* zoom in y axis */
void  point_magy(int nelem, point *points, real ymag)
{
  int  ii;

  for(ii = 0; ii < nelem; ii++)
    points[ii].ycord *= ymag;

  return;
}  /* endof point_magy() */


/* zoom out points */
void  point_zoom(int nelem, point *points, real zoom)
{
  int  ii;

  for(ii = 0; ii < nelem; ii++)
  {
    points[ii].xcord *= zoom;
    points[ii].ycord *= zoom;
  }

  return;
}  /* endof point_zoom() */


/* shift along x axis */
void  point_shiftx(int nelem, point *points, real xshift)
{
  int  ii;

  for(ii = 0; ii < nelem; ii++)
    points[ii].xcord += xshift;

  return;
}  /* endof point_shiftx() */


/* shift along y axis */
void  point_shifty(int nelem, point *points, real yshift)
{
  int  ii;

  for(ii = 0; ii < nelem; ii++)
    points[ii].ycord += yshift;

  return;
}  /* endof point_shiftx() */


/* rotate points clockwise around the origo (rotation angle in radians) */
void  point_rotate(int nelem, point *points, real rotang)
{
  int   ii;
  real  xcord, ycord;
  real  cphi = cos(rotang);
  real  sphi = sin(rotang);

  for(ii = 0; ii < nelem; ii++)
  {
    xcord = points[ii].xcord;
    ycord = points[ii].ycord;

    points[ii].xcord =  cphi*xcord + sphi*ycord;
    points[ii].ycord = -sphi*xcord + cphi*ycord;
  }

  return;
}  /* endof point_rotate() */


/* rotate points clockwise around the origo (rotation angle in degrees) */
void  point_rrotate(int nelem, point *points, real rotang)
{
  point_rotate(nelem, points, DEG2RAD(rotang));
  return;
}  /* endof point_rrotate() */

/* sort points based on properties (for qsort()) */
static int point_sortprop( const void* p1, const void* p2 )
{
  real  prop1 = ((const point *) p1)->prop;
  real  prop2 = ((const point *) p2)->prop;

  if ( IS_EQUAL( prop1, prop2 ) )
    return  0;

  return  prop1 < prop2 ? -1 : 1;
}

static int point_sortprop_desc( const void* p1, const void* p2 )
{
  real  prop1 = ((const point *) p1)->prop;
  real  prop2 = ((const point *) p2)->prop;

  if ( IS_EQUAL( prop1, prop2 ) )
    return  0;

  return  prop1 < prop2 ? 1 : -1;
}

void pointsortprop( point* arr, size_t arrlen, bool ascending )
{
  if ( ascending ) {
    qsort( arr, arrlen, sizeof *arr, point_sortprop );
  }
  else {
    qsort( arr, arrlen, sizeof *arr, point_sortprop_desc );
  }
}

/* copy a point array into another one */
point  *point_copy(int nelem, point *points)
{
  point  *dest;
  size_t  size = nelem*sizeof *dest;

  if(nelem < 1 || points == NULL)
    return  NULL;

  if((dest = malloc(size)) != NULL)
    memcpy((void *) dest, (void *) points, size);

  return  dest;
}  /* endof point_copy() */


/* remove a point from an array */
void  point_remove(int idx, int nelem, point *points)
{
  if(nelem > 1 && points != NULL && idx > 0 && idx < --nelem)
  {
    points += idx;
    memmove((void *) points, (void *) (points + 1),
	    (nelem - idx)*sizeof *points);
  }

  return;
}  /* endof point_remove() */


const point* point_closestpt(
    const point*    arr,
    size_t          nelem,
    const point*    refpoint
)
{
    if ( arr == NULL || refpoint == NULL || nelem < 1 ) {
        return NULL;
    }

    real mindist = point_eucdist2( arr, refpoint );
    const point* closest = arr;

    for ( size_t idx = 1; idx < nelem; ++idx ) {
        real dist = point_eucdist2( &arr[idx], refpoint );
        if ( dist < mindist ) {
            mindist = dist;
            closest = &arr[idx];
        }
    }

    return closest;
}
