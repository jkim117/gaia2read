#ifndef GAIA2_RET_H__
#define GAIA2_RET_H__

#include "gaiastar.h"
#include "sllist.h"

typedef enum
{
	GAIA,HAT,TMASS
} IDType;

typedef struct {
    const double* minGmag;
    const double* maxGmag;
    const double* minRpmag;
    const double* maxRpmag;
    const double* minBpmag;
    const double* maxBpmag;
} maglims;

// returns count of stars in for the size of an array
int starPosCount(double ra, double dec, bool circle, double frame_size, const double *epoch,
        const maglims* maglim);

// returns list of stars given size, circle or rectangular, and center ra and dec
int starPosSearch(double ra, double dec, bool circle, double frame_size, const double *epoch,
        const maglims* maglim, gaiastar* stars);

// get list of stars from a list of Gaia IDs
int starsfromID(sllist* longIDs, const double *epoch,gaiastar* stars);

//from starlist to either an array of hat or 2mass ids
sllist* starListToIDs(gaiastar* stars, IDType outID,int count);

// should work on all id types, including a gaia, in which it just returns otherIDt
char* toGaiaID(const char* otherID, IDType inID, char* buffer);

// checks to make sure star is within box and applies proper motion
bool test_star(gaiastar* star, double centRA, double centDec, double half_size, const double *epoch,
        const maglims* maglim);

// tests star to make sure it is within the circle and then applies proper motion
bool test_starcirc(gaiastar* star,double centRA,double centDec,double radius, const double *epoch,
        const maglims* maglim);

#endif

