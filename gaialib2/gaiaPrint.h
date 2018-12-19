#include <stdbool.h>

#include "gaia2ret.h"
#include "sllist.h"
#include "astrometry.h"

// print list of stars with Gaia ID
void gaiastar_printlist(FILE* out, const gaiastar stars[], int count, bool extra, skypos* xieta_center);

// print list of stars with specified ID type
void gaiastar_printlist_alternateID(FILE* out, const gaiastar stars[], int count, bool extra, const sllist* alternateIDs, IDType type, skypos* xieta_center);

// print header
void gaiastar_printheader(FILE* out, bool extra, IDType outType, bool print_xieta);

