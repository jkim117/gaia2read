#include <stdbool.h>

typedef bool (*testfunc) (
    gaiastar*          star,
    double             centRA,
    double             centDec,
    double             half_size,
    const double*       epoch
);

int posQuery(double raMin, double raMax, double decMin, double decMax, testfunc tester,double ra,double dec, double frame_size, const double *epoch,gaiastar stars[]);

int posCount(double raMin, double raMax, double decMin, double decMax, testfunc tester,double ra,double dec, double frame_size, const double *epoch);

void gaia2_setpath( const char* path );

void gaia2_getpath();
