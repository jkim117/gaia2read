/* myargs.h : handle command line arguments (domsa@konkoly.hu (2002)) */

#ifndef MY_ARGS_H__
#define MY_ARGS_H__

#ifdef __cplusplus
extern "C" {
#endif 

/** \file myargs.h
    \brief Functions parseing command line arguments and config files
*/


/** error codes returned by mygetopt() */
enum {
  NO_MORE_OPTIONS = -1, INVALID_OPTION = -2, MISSING_ARGUMENT = -3
} mygetopt_errorcodes;


/** public index of the current cmd-line option (see getopt() manual) */
int         myoptind;

/** public pointer to the current cmd-line string (see getopt() manual) */
const char* myoptarg;


/** long option argument types 

    Long options start with double dash, like --help .
*/
typedef enum { no_argument, required_argument, optional_argument } arg_type;


/** struct to hold long options 

    Long options start with double dash, like --help .
    For more information see getopt() manual.
*/
typedef struct
{
  /** option name */
  const char*   opname;

  /** argument type */
  arg_type      atyp;

  /** value returned from mygetopt(), when encountered with this option */
  int           retval;
}  myoptlong;


/** data types in a configuration file */
typedef enum {
  char_type, bool_type, int_type, real_type
} conf_type;


/** error codes from getconfile() */
typedef enum {
  no_error, no_confile, no_options, parse_error,
  invalid_option, invalid_argument, invalid_type 
} conf_errcode;


/** config file entries */
typedef struct
{
  /** option name */
  char       *opname;

  /** options data type */
  conf_type  type;

  /** write its value to a void pointer */
  void       *value;
} conf_struct;


/** handle command line arguments

    The function takes the command line arguments in a form a an array of
    pointers to strings, and look for short (specified by \a ops) and
    long options (in \a longoptions), returning the code of the option if 
    it was found on the command line. When no more option's left, it returns
    NO_MORE_OPTIONS. (For other error codes see mygetopt_errorcodes.) The
    options may be intermixed with other arguments.

    The function modifies the the original order in the \a argv list, but
    options are evaluated in the same order as they appeared on the command
    line, with non-options arguments following them. For more documentation 
    see getopt() manual.

    \param argc  number of command line arguments
    \param argv  array of the command line arguments
    \param ops   list of short options as a string. An option followed
    by a semicolon means that the option expects an argument. Example
    "f:h", means that the -f option requires an argument, but the -h
    do not.
    \param longoptions a NULL terminated array of myoptlong structs,
    containing the options name, its argument type, and the code for the
    long option.
*/
int     mygetopt(
            int                 argc,
            char**              argv,
            const char*         ops,
            const myoptlong*    longoptions
        );

void    margs_pushbackarg();

/** parse a configuration file 

    getconfile() opens a configuration file, and looks for matching
    parameters as defined in \a cs.

    The general format of the parameter file looks like \n
    PARAMETER = VALUE
    where each parameter appears on a different line. There may be
    empty or commented out lines in the configuration file (lines starting 
    with a hashmark).

    \param configfile  name of the configuration file
    \param cs  the conf_struct array containing the options for the parameters.
*/
conf_errcode  getconfile(const char* configfile, conf_struct *cs);

bool    get_colpos( const char* arg, int* n1, int* n2 );
bool    get_coldpos( const char* arg, real* n1, real* n2 );
bool    get_coldposex( const char* text, real* val1, real* val2 );

/** parse argument <n1,n2[,n3]> */
bool    get_colpos3( const char* arg, int* n1, int* n2, int* n3 );

bool    margs_get_area( const char* arg, real* area );

bool    is_parameter( const char* str );

bool    is_shell_printable( const char* str );
void    myargs_print_cmdline( FILE* out, int argc, char** argv );

/** terminates program with error message with current argument */
void    myargs_abort( const char* format );

/** scans integer or floating point values from command line */
bool    myargs_int( int* val );
bool    myargs_real( real* val );

/** open file with mode */
FILE*   myargs_fopen( const char* mode );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* MY_ARGS_H__ */
