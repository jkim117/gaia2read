
#include "astrio.h"
#include "astromath.h"
#include "mmath.h"
#include "gaia2ret.h"

#include <string.h>
#include <time.h>

/* ========================================================================= */

/* converts degrees to sexagesimal representation (hh:mm:ss.ss) */
void deg2hms( real deg, char* out, size_t buf_len )
{
    int hour, min;
    real sec;

    deg2HMS( deg, &hour, &min, &sec );
    snprintf( out, buf_len, "%02d:%02d:%06.3f", hour, min, sec );
}

/* ------------------------------------------------------------------------- */

/* converts sexagesimal representation to degrees (hh:mm:ss.ss) */
bool hms2deg( const char* in, real* deg )
{
    char hours[MAX_WORD], mins[MAX_WORD], secs[MAX_WORD];
    int hour, min;
    real sec;

    if ( in == NULL || strlen( in ) == 0 || strlen( in ) >= MAX_WORD ) {
        return false;   /* sanity check failed */
    }

    if (    colcut( hours, in, 0, ":" ) == false
        ||  colcut( mins, in, 1, ":" )  == false
        ||  colcut( secs, in, 2, ":" )  == false
    ) {
        return false;   /* input has invalid format */
    }

    if (    mystr2i( hours, &hour ) == false
        ||  mystr2i( mins, &min )   == false
        ||  mystr2d( secs, &sec )   == false
    ) {
        return false;   /* input has invalid format */
    }

    hour = dmodi( hour, 24.0 );
    *deg = HOUR2DEG( hour + MIN2DEC( min + MIN2DEC( sec ) ) );

    return  true;
}

/* ------------------------------------------------------------------------- */

/* converts degrees to sexagesimal representation (dd:mm:ss.ss) */
void deg2dms( real deg, char* out, size_t buf_len )
{
    int dout, min;
    real sec;
    deg2DMS( deg, &dout, &min, &sec );

    bool is_negative = false;

    if ( dout < 0 ) {
        is_negative = true;
        dout = ABS( dout );
    }

    if ( min < 0 ) {
        is_negative = true;
        min = ABS( min );
    }

    if ( IS_NEGATIVE( sec ) ) {
        is_negative = true;
        sec = ABS( sec );
    }

    if ( !is_negative ) {
        snprintf( out, buf_len, " %02d:%02d:%06.3f", dout, min, sec );
    }
    else {
        snprintf( out, buf_len, "-%02d:%02d:%06.3f", dout, min, sec );
    }
}

/* ------------------------------------------------------------------------- */

/* converts degrees in sexagesimal representation (dd:mm:ss.ss) to degrees */
bool dms2deg( const char* in, real* dout )
{
    char degs[MAX_WORD], mins[MAX_WORD], secs[MAX_WORD];
    int deg, min;
    real sec;

    if ( in == NULL || strlen( in ) == 0 || strlen( in ) >= MAX_WORD ) {
        return false;   /* sanity check failed */
    }

    if (    colcut( degs, in, 0, ":" )  == false
        ||  colcut( mins, in, 1, ":" )  == false
        ||  colcut( secs, in, 2, ":" )  == false
    ) {
        return false;   /* input has invalid format */
    }

    if (    mystr2i( degs, &deg )   == false
        ||  mystr2i( mins, &min )   == false
        ||  mystr2d( secs, &sec )   == false
    ) {
        return false;   /* input has invalid format */
    }

    deg = mfloor( dmodDec( deg ) );
    sec = MIN2DEC( min + MIN2DEC( sec ) );
    if ( deg < 0 || degs[0] == '-' ) {
        *dout = deg - sec;
    }
    else {
        *dout = deg + sec;
    }

    return  true;
}

/* ========================================================================= */

/* formats JD as date time */
void astrio_jdnprintf( char* txt, size_t len, real jd )
{
    int year, month, day, hour, min;
    real sec;

    jd2caldate( jd, &year, &month, &day, &hour, &min, &sec );

    snprintf(
        txt, len,
        "%04d-%02d-%02d %02d:%02d:%05.2f",
        year, month, day, hour, min, sec
    );
}

/* ------------------------------------------------------------------------- */

bool astrio_text2jd( const char* text, real* jd, DateFormat* fmt )
{
    if ( fmt ) {
        *fmt = DATEFMT_UNKNOWN;
    }

    if ( mystr2d( text, jd ) ) {
        if ( fmt ) {
            *fmt = DATEFMT_JD;
        }

        return true;        // JD
    }
    else if ( strncmp( text, "MJD", 3 ) == 0 ) {
        const char* mjd = text + 3;
        if ( mystr2d( mjd, jd ) ) {
            *jd = mjd2jd( *jd );

            if ( fmt ) {
                *fmt = DATEFMT_MJD;
            }

            return true;    // modified JD
        }
        else {
            return false;
        }
    }
    else if ( *text == 'J' ) {
        real year;
        if ( mystr2d( text + 1, &year ) ) {
            *jd = epoch2jd( year );

            if ( fmt ) {
                *fmt = DATEFMT_EPOCH;
            }

            return true;    // Julian year: JYYYY.YY
        }
        else {
            return false;
        }
    }
    else if ( eqstring( text, "NOW" ) ) {
        time_t rawtime = time( NULL );
        struct tm* gm = gmtime( &rawtime );
        *jd = juliandate(
            gm->tm_year + 1900, gm->tm_mon + 1, gm->tm_mday,
            gm->tm_hour, gm->tm_min, gm->tm_sec
        );

        if ( fmt ) {
            *fmt = DATEFMT_CALDATE;
        }

        return true;        // current time (UT)
    }

    const char* dt_sep = " -:";
    char piece[strlen( text ) + 1];

    int year;
    if ( !colcut( piece, text, 0, dt_sep ) )
        return false;
    if ( !mystr2i( piece, &year ) )
        return false;

    int month;
    if ( !colcut( piece, text, 1, dt_sep ) )
        return false;
    if ( !mystr2i( piece, &month ) )
        return false;

    int day;
    if ( !colcut( piece, text, 2, dt_sep ) )
        return false;
    if ( !mystr2i( piece, &day ) )
        return false;

    int hour = 0;       // hour is optional
    if ( colcut( piece, text, 3, dt_sep ) ) {
        if ( !mystr2i( piece, &hour ) ) {
            return false;
        }
    }

    int min = 0;        // min is optional
    if ( colcut( piece, text, 4, dt_sep ) ) {
        if ( !mystr2i( piece, &min ) ) {
            return false;
        }
    }

    real sec = 0.0;     // sec is optional
    if ( colcut( piece, text, 5, dt_sep ) ) {
        if ( !mystr2d( piece, &sec ) ) {
            return false;
        }
    }

    *jd = juliandate( year, month, day, hour, min, sec );
    if ( fmt ) {
        *fmt = DATEFMT_CALDATE;
    }

    return true;
}

/* ------------------------------------------------------------------------- */

void astrio_prskydeg( FILE* out, const skypos* pos )
{
    fprintf( out, "%11.7f %11.7f", pos->RA, pos->Dec );
}

/* ------------------------------------------------------------------------- */

bool astrio_parsepos( const char* text, skypos* pos, bool* in_sexagesimal )
{
    if ( in_sexagesimal ) {
        *in_sexagesimal = false;
    }

    char sRA[MAX_WORD], sDec[MAX_WORD];
    if (    colcut( sRA, text, 0, " ," ) == false
        ||  colcut( sDec, text, 1, " ," ) == false
    ) {
        return false;
    }

    if ( !astrio_parseRA( sRA, &pos->RA, in_sexagesimal ) ) {
        return false;
    }

    if ( !astrio_parseDec( sDec, &pos->Dec, NULL ) ) {
        return false;
    }

    return true;
}

bool astrio_parseRA( const char* text, real* ra, bool* in_sexagesimal )
{
    if ( mystr2d( text, ra) == false ) {
        if ( !hms2deg( text, ra ) ) {
            return false;
        }

        if ( in_sexagesimal ) {
            *in_sexagesimal = true;
        }
    }

    return true;
}

bool astrio_parseDec( const char* text, real* dec, bool* in_sexagesimal )
{
    if ( mystr2d( text, dec ) == false ) {
        if ( !dms2deg( text, dec ) ) {
            return false;
        }

        if ( in_sexagesimal ) {
            *in_sexagesimal = true;
        }
    }

    return true;
}

bool astrio_parseID( const char* text, IDType* type, bool* in_sexagesimal )
{
    if(strcmp(text,"HAT")==0)
    {
        *type = HAT;
        if ( in_sexagesimal )
            *in_sexagesimal = true;
        return true;
    }
    else if(strcmp(text,"TMASS")==0)
    {
        *type = TMASS;
        if ( in_sexagesimal )
            *in_sexagesimal = true;
        return true;
    }
    else if(strcmp(text,"GAIA")==0)
    {
        *type = GAIA;
        if ( in_sexagesimal )
            *in_sexagesimal = true;
        return true;
    }

    return false;
}





/* ========================================================================= */
