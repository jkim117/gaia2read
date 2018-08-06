/* point.h : operation on 2D points and arrays (domsa@konkoly.hu (2002)) */

#ifndef POINT_H__
#define POINT_H__

/** \file point.h
    \brief Operation with two dimensional points.

    Definition of 2D points and arrays, and their utility functions.
*/


#include "utils.h"


/** store a 2D point */
typedef struct
{
  /** id number to identify the point */
  int   id;

  /** its x and y coordinates */
  real  xcord, ycord;

  /** arbitary property of a point */
  real  prop;

  char* line;
}  point;


/** an array of 2D points (see point) */
typedef struct
{
  /** array length */
  int    length;

  /** pointer to the array */
  point  *points;
}  pointarr;



/* exported functions */


/** create an empty pointarr struct */
pointarr  *point_createarr(void);

/** build a pointarr from a point vector

    Create an pointarr struct, and set its members equal to the input values.
    No check on the parameters.

    \param length  length of the array
    \param points  the point vector
*/
pointarr  *point_buildarr(int length, point *points);


/** destroy a point array

    The function deallocate the vector member of the pointarr struct,
    and deallocate the struct itself.

    \param pa  pointarr to be destroyed
*/
void      point_destroyarr(pointarr *pa);


/** calculate eucledian distance between two 2D points

    function returns the eucledian distance
    \f$d = \sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}\f$

    \todo use a hypot() implementation instead of calling sqrt()

    \param p1 first point
    \param p2 second point
*/
real  point_eucdist( const point* p1, const point* p2 );


/** calculate squared eucledian distance between two 2D points

    function returns the squared eucledian distance
    \f$d = (x_1 - x_2)^2 + (y_1 - y_2)^2\f$

    \param p1 first point
    \param p2 second point
*/
real  point_eucdist2( const point* p1, const point* p2 );

real  point_abs( const point* pt );


/** return the minimum of x coordinates in a point array

    return 0 on error

    \param nelem  length of the array
    \param points  point vector
*/
real  point_minx(int nelem, point *points);


/** return the minimum of y coordinates in a point array

    return 0 on error

    \param nelem  length of the array
    \param points  point vector
*/
real  point_miny(int nelem, point *points);


/** return the maximum of x coordinates in a point array

    return 0 on error

    \param nelem  length of the array
    \param points  point vector
*/
real  point_maxx(int nelem, point *points);


/** return the maximum of y coordinates in a point array

    return 0 on error

    \param nelem  length of the array
    \param points  point vector
*/
real  point_maxy(int nelem, point *points);


/** return the minimum and maximum x coordinate values in a point array

    \todo add error checking

    \param nelem  length of the array
    \param points  point vector
    \param  minx   returned: minimum of the x coordinates
    \param  maxx   returned: maximum of the x coordinates
*/
void  point_minmaxx(int nelem, point *points, real *minx, real *maxx);


/** return the minimum and maximum y coordinate values in a point array

    \todo add error checking

    \param nelem  length of the array
    \param points  point vector
    \param  miny   returned: minimum of the y coordinates
    \param  maxy   returned: maximum of the y coordinates
*/
void  point_minmaxy(int nelem, point *points, real *miny, real *maxy);


/** calculate of the span of the x coordinates in a point array

    \todo add error checking

    \param nelem length of the array
    \param points  point vector
*/
real  point_lengthx(int nelem, point *points);


/** calculate of the span of they y coordinates in a point array

    \todo add error checking

    \param nelem length of the array
    \param points  point vector
*/
real  point_lengthy(int nelem, point *points);


/** flip the x coordinates around the center of the array

    \todo add error checking

    \param nelem length of the array
    \param points  point vector
*/
void  point_flipx(int nelem, point *points);


/** flip the y coordinates around the center of the array

    \todo add error checking

    \param nelem length of the array
    \param points  point vector
*/
void  point_flipy(int nelem, point *points);


/** zoom points in the x coordinates

    multiply x coordinates with the \a xmag factor

    \param nelem length of the array
    \param points  point vector
    \param xmag  zooming factor
*/
void  point_magx(int nelem, point *points, real xmag);


/** zoom points in the y coordinates

    multiply y coordinates with the \a ymag factor

    \param nelem length of the array
    \param points  point vector
    \param ymag  zooming factor
*/
void  point_magy(int nelem, point *points, real ymag);


/** zoom points in both coordinates

    multiply x and y coordinates with \a zoom

    \param nelem length of the array
    \param points point vector
    \param zoom  zooming factor
 */
void  point_zoom(int nelem, point *points, real zoom);


/** shift points along the x axis

    \param nelem  length of the array
    \param points point vector
    \param xshift value of the shift
*/
void  point_shiftx(int nelem, point *points, real xshift);


/** shift points along the y axis

    \param nelem  length of the array
    \param points point vector
    \param yshift value of the shift
*/
void  point_shifty(int nelem, point *points, real yshift);


/* rotate points clockwise around the origo (rotation angle in radians) */
void  point_rotate(int /*nelem*/, point * /*points*/, real /*rotang*/);

/* rotate points clockwise around the origo (rotation angle in degrees) */
void  point_rrotate(int /*nelem*/, point * /*points*/, real /*rotang*/);

/* sort array on properties */
void pointsortprop( point* arr, size_t arrlen, bool ascending );

/* copy a point array into a new one */
point  *point_copy(int /*nelem*/, point * /*points*/);

/* remove a point from an array */
void  point_remove(int /*idx*/, int /*nelem*/, point * /*points*/);

/* find closest point in an array to reference point */
const point* point_closestpt(
    const point*    arr,
    size_t          nelem,
    const point*    refpoint
);

#endif /* POINT_H__ */

