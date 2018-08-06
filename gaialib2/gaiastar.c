#include <stdbool.h>

#include "gaiastar.h"
#include "pmotion.h"

// compare function for two gaia stars to see which as the greater ra value. Used for sorting
int starcmp(const void * a, const void * b)
{
  gaiastar* s1 = (gaiastar*)a;
  gaiastar* s2 = (gaiastar*)b;
  double result = (s1->ra) - (s2->ra);
  if (result > 0.0)
    return 1;
  if (result < 0.0)
    return -1;
  else
    return 0;
}

// precess according to given equinox
void gaia2_precesslist(gaiastar stars[], double JD/* Julian days */,int count)
{
  for(int i = 0; i < count; i++)
    {
      gaiastar* star = &stars[i];
      pmotion_precess2000(&star->ra,&star->dec,JD,0,0,true);
    }
}

