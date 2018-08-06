
#ifndef ASTR_IO_H__
#define ASTR_IO_H__

#include "utils.h"
#include "astrometry.h"
#include "gaia2ret.h"

/* ========================================================================= */

#define HMS_FMT_LENGTH      20      /* hour:min:sec format length */
#define CALDAT_FMT_LENGTH   23      /* YYYY-MM-DD HH:MM:SS.SS */

typedef enum DateFormat {
    DATEFMT_CALDATE,
    DATEFMT_JD,
    DATEFMT_MJD,
    DATEFMT_EPOCH,
    DATEFMT_UNKNOWN
} DateFormat;

/* ========================================================================= */

/* converts degrees to sexagesimal representation (hh:mm:ss.ss) */
void    deg2hms( real deg, char* out, size_t buf_len );

/* converts sexagesimal representation to degrees (hh:mm:ss.ss) */
bool    hms2deg( const char* in, real* deg );

/* converts degrees to sexagesimal representation (dd:mm:ss.ss) */
void    deg2dms( real deg, char* out, size_t buf_len );

/* converts degrees in sexagesimal representation (dd:mm:ss.ss) to degrees */
bool    dms2deg( const char* in, real* dout );

/* ========================================================================= */

/* formats JD as date time */
void    astrio_jdnprintf( char* txt, size_t len, real jd );

/* converts calendar date-time to JD */
bool    astrio_text2jd( const char* text, real* jd, DateFormat* fmt );

/* print formatted sky coordinates to stream */
void    astrio_prskydeg( FILE* out, const skypos* pos );

/* parse sky position to skypos: RA and Dec is space or coma separated */
bool    astrio_parsepos( const char* text, skypos* pos, bool* in_sexagesimal );

/* parse RA to deg */
bool    astrio_parseRA( const char* text, real* ra, bool* in_sexagesimal );

/* parse Dec to deg */
bool    astrio_parseDec( const char* text, real* dec, bool* in_sexagesimal );

/* parse ID type */
bool    astrio_parseID( const char* text, IDType* type, bool* in_sexagesimal );

/* ========================================================================= */
#endif /* ASTR_IO_H__ */
