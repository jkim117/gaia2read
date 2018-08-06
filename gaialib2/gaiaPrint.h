#include <stdbool.h>

#include "gaia2ret.h"
#include "sllist.h"

// print list of stars with Gaia ID
void gaiastar_printlist(FILE* out,const gaiastar stars[], bool extra,int count);

// print list of stars with specified ID type
void gaiastar_printlist_alternateID(FILE* out, const gaiastar stars[], bool extra, const sllist* alternateIDs, IDType type,int count);

// print header
void gaiastar_printheader(FILE* out, bool extra, IDType outType);

