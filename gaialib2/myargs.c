/* myargs.c : handle command line arguments (domsa@konkoly.hu (2002)) */

#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "myargs.h"



/* local functions */
static int  long_option(int, int, char **, const myoptlong *, bool*);
static int  short_option(int, int, char **, const char *, bool*);

static int  aind   = 1;


/* global function */
int mygetopt(
    int                 argc,
    char**              argv,
    const char*         sops,
    const myoptlong*    lops
)
{
    int retval = NO_MORE_OPTIONS;
    bool arg_present = true;
    bool option_found = false;

    for ( int ind = aind; ind < argc && !option_found; ind++ ) {
        if ( is_parameter( argv[ind] ) ) {
            if ( argv[ind][1] == '-' ) {
                retval = long_option( ind, argc, argv, lops, &arg_present );
            }
            else {
                retval = short_option( ind, argc, argv, sops, &arg_present );
            }

            /* error parsing options */
            if ( retval < NO_MORE_OPTIONS ) {
                if ( retval == INVALID_OPTION ) {
                    err_print_msg( "invalid option: %s", argv[ind] );
                }
                else if ( retval == MISSING_ARGUMENT ) {
                    err_print_msg( "missing argument to option %s", argv[ind] );
                }
            }

            /* move arguments up */
            int idx = ind;
            int shift_arg = arg_present ? 1 : 0;

            do {
                while ( idx > aind ) {
                    SWAP( argv[idx], argv[idx - 1], char* );
                    idx--;
                }
                aind++;
            } while ( shift_arg-- && ( idx = ind + 1 ) );

            /* an option was found */
            option_found = true;
        }
    }

    myoptind = option_found ? aind - 1 : aind;
    myoptarg = arg_present ? argv[myoptind] : NULL;

    return  retval;
}

void margs_pushbackarg()
{
    --aind;
}

/* read in config file */
conf_errcode getconfile( const char* filename, conf_struct* cf )
{
  char          line[MAX_LINE], keyword[MAX_LINE], kvalue[MAX_LINE];
  const char*   cp;
  conf_struct*  ops;
  stream        cfile;


  if(filename == NULL || cf == NULL)
    return  no_options;

  /* open config file */
  if((cfile = fopen(filename, "r")) == NULL)
    return  no_confile;


  /* parse config file */
  while(fgets(line, sizeof(line), cfile) != NULL)
  {
    /* skip empty lines or comments */
    if((cp = empty_string(line)) == NULL || *cp == '#')
      continue;

    if(colcut(keyword, line, 0, "=") == false
       || colcut(kvalue, line, 1, "=") == false)
    {
      fclose(cfile);
      return  parse_error;
    }

    /* search for the given keyword in the table */
    for(ops = cf; ops->opname != NULL; ops++)
      if(eqstring(keyword, ops->opname))
	break;

    if(ops->opname == NULL)
    {
      fclose(cfile);
      return  invalid_option;
    }

    /* convert argument */
    switch(ops->type)
    {
      int   tmpint;
      real  tmpreal;

    case char_type:
      strcpy(( char* )ops->value, kvalue);
      break;

    case bool_type:
      *((bool *) ops->value) = *kvalue == 'T' ? true : false;
      break;

    case int_type:
      if(mystr2i(kvalue, &tmpint) == false)
      {
	fclose(cfile);
	return  invalid_argument;
      }

      *((int *) ops->value) = tmpint;
      break;

    case real_type:
      if(mystr2d(kvalue, &tmpreal) == false)
      {
	fclose(cfile);
	return  invalid_argument;
      }

      *((real *) ops->value) = tmpreal;
      break;

    default:
      fclose(cfile);
      return  invalid_type;
    }
  }

  fclose(cfile);

  return  no_error;
}  /* endof conf_errcode() */



/* local functions */

/* deal with long options */
static int  long_option(
    int                 ind,
    int                 argc,
    char**              argv,
    const myoptlong*    lops,
    bool*               arg_there  /*returned: */
)
{
  const char* opname = &argv[ind][2];
  const char* curarg;
  int   ii;

  *arg_there = false;

  if(lops == NULL)
    return  INVALID_OPTION;

  for(ii = 0; (curarg = lops[ii].opname) != 0; ii++)
    if(eqstring(opname, curarg))
      break;

  if(curarg == 0)
    return  INVALID_OPTION;


  if(lops[ii].atyp == required_argument)
  {
    if(argc == ind + 1)
      return  MISSING_ARGUMENT;
    else
      *arg_there = true;
  }
  else if ( lops[ii].atyp == optional_argument ) {
    if ( argc != ind + 1 && !is_parameter( argv[ind + 1] ) ) {
      *arg_there = true;
    }
  }


  return  lops[ii].retval;
}  /* endof long_option() */


/* deal with short options */
static int  short_option(
    int             ind,
    int             argc,
    char**          argv,
    const char*     sops,
    bool*           arg_there   /*returned:*/
)
{
  int         ii;
  static int  len = 0;

  if(!len)
    len = strlen(sops);

  *arg_there = false;

  for(ii = 0; ii < len; ii++)
    if(sops[ii] != ':' && argv[ind][1] == sops[ii])
      break;

  if(ii == len)
    return  INVALID_OPTION;

  if(sops[ii+1] == ':')
  {
    if(argc == ind + 1)
      return  MISSING_ARGUMENT;
    else
      *arg_there = true;
  }
    else if ( sops[ii + 1] == '?' ) {
        if ( argc != ind + 1 && !is_parameter( argv[ind + 1] ) ) {
            *arg_there = true;
        }
    }

  return  sops[ii];
}  /* endof short_option() */


bool get_colpos( const char* arg, int* n1, int* n2 )
{
    if ( arg == NULL )
        return false;

    *n1 = 0;
    *n2 = 0;

    int arglen = strlen( arg );
    char* sep = strchr( arg, ',' );
    if ( !sep || sep == arg || sep - arg == arglen - 1 )
        return false;

    *sep = '\0';

    bool ret = mystr2i( arg, n1 );
    ret = ret && mystr2i( sep + 1, n2 );

    *sep = ',';
    return ret;
}

bool get_colpos3( const char* arg, int* n1, int* n2, int* n3 )
{
    if ( arg == NULL )
        return false;

    *n1 = -1;
    *n2 = -1;
    *n3 = -1;

    int arglen = strlen( arg );
    char* sep = strchr( arg, ',' );

    if ( !sep ) {
        return mystr2i( arg, n1 );
    }

    if ( sep == arg || sep - arg == arglen - 1 )
        return false;

    *sep = '\0';

    if ( !mystr2i( arg, n1 ) ) {
        return false;
    }

    *sep = ',';
    char* sep2 = strchr( sep + 1, ',' );
    if ( !sep2 ) {
        return mystr2i( sep + 1, n2 );
    }
    else {
        *sep2 = '\0';
        bool res = mystr2i( sep + 1, n2 );
        *sep2 = ',';
        if ( !res ) {
            return false;
        }
    }

    return mystr2i( sep2 + 1, n3 );
}

bool get_coldpos( const char* arg, real* n1, real* n2 )
{
    if ( arg == NULL )
        return false;

    *n1 = 0;
    *n2 = 0;

    int arglen = strlen( arg );
    char* sep = strchr( arg, ',' );
    if ( !sep || sep == arg || sep - arg == arglen - 1 )
        return false;

    *sep = '\0';

    bool ret = mystr2d( arg, n1 );
    ret = ret && mystr2d( sep + 1, n2 );

    *sep = ',';
    return ret;
}

bool get_coldposex( const char* text, real* val1, real* val2 )
{
    const char* dt_sep = " ,";
    char piece[strlen( text ) + 1];

    if ( !colcut( piece, text, 0, dt_sep ) )
        return false;
    if ( !mystr2d( piece, val1 ) )
        return false;

    if ( !colcut( piece, text, 1, dt_sep ) )
        return false;
    if ( !mystr2d( piece, val2 ) )
        return false;

    return true;
}

// get [x1:x2,y1:y2]
bool margs_get_area( const char* arg, real* area )
{
    static const char* areasep = "\',[:]";

    char lbx[MAX_WORD];
    char lby[MAX_WORD];
    char rtx[MAX_WORD];
    char rty[MAX_WORD];

    if (    arg == NULL
        ||  !colcut( lbx, arg, 0, areasep )
        ||  !colcut( lby, arg, 2, areasep )
        ||  !colcut( rtx, arg, 1, areasep )
        ||  !colcut( rty, arg, 3, areasep )
        ||  !mystr2d( lbx, &area[0] )
        ||  !mystr2d( lby, &area[1] )
        ||  !mystr2d( rtx, &area[2] )
        ||  !mystr2d( rty, &area[3] )
    ) {
        return false;
    }

    return true;
}

bool is_parameter( const char* str )
{
    if ( !str || *str != '-' ) {
      return false;
    }

    real dummy;
    if ( mystr2d( str, &dummy ) ) {
      // negative number in the command line
      return false;
    }

    return true;
}

bool is_shell_printable( const char* str )
{
    const char* nonprintable_chars = " ()*?[]&|;";
    return strpbrk( str, nonprintable_chars ) == NULL;
}

void myargs_print_cmdline( FILE* out, int argc, char** argv )
{
    for ( int idx = 0; idx < argc; ++idx ) {
        if ( idx ) {
            fputc( ' ', out );
        }

        if ( is_shell_printable( argv[idx] ) ) {
            fputs( argv[idx], out );
        }
        else {
            fputc( '"', out );
            fputs( argv[idx], out );
            fputc( '"', out );
        }
    }

    fendl( out );
}

void myargs_abort( const char* format )
{
    err_ret_failure( format, myoptarg );
}

bool myargs_int( int* val )
{
    return mystr2i( myoptarg, val );
}

bool myargs_real( real* val )
{
    return mystr2d( myoptarg, val );
}

FILE* myargs_fopen( const char* mode )
{
    FILE* fstream = fopen( myoptarg, mode );
    if ( !fstream ) {
        myargs_abort( "cannot open file %s" );
    }

    return fstream;
}
