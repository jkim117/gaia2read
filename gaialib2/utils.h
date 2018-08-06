/* utils.h : misc functions and definitions (domsa@konkoly.hu (2002)) */

#ifndef UTILS_H__
#define UTILS_H__


/** \file utils.h                                                                                                                                                                                                                                                               
    \brief  Miscallenous macros, definitions and functions.                                                                                                                                                                                                                     
                                                                                                                                                                                                                                                                                
    It is included in almost all source files.                                                                                                                                                                                                                                  
*/



#include <stdio.h>
#include <stdbool.h>


/** use GNU's extension __FUNCTION__ (or "unused function") */
#if !defined(__GNUC__) && !defined(__FUNCTION__)
#  define __FUNCTION__  "unknown function"
#endif


/** print error message \a x to the stdout, along with the function name */
#define SCREAM(x)   fprintf(stderr, "%s(): %s\n", __FUNCTION__, (x))


#if !defined(local) && !defined(global)
/** use local as a synonim to "static" */
# define local      static
/** global is set to empty string */
# define global
#endif


/** shortcut to stdio's file handler */
typedef FILE*       stream;


/** stringification */
#define Str(x)      #x

/** catenate strings */
#define Xstr(x)     Str(x)


/** compare two strings \a x and \a y. qstrcmp() is a wrapper to strcmp() */
#define qstrcmp(x,y)    (*(x) == *(y) ? strcmp((x), (y)) : *(x) - *(y))

/** check if strings \a x and \a y equal each other */
#define eqstring(x,y)   (*(x) == *(y) ? (strcmp((x), (y)) == 0) : 0)

/** check if string \a x is empty */
#define nullstr(x)      (*(x) == '\0')


/** real is either a double or a float (\a SINGLEPREC defined) */
#ifndef REAL_TYPE
# define REAL_TYPE
# ifndef SINGLEPREC
typedef double  real;
# else
typedef float   real;
# endif
#endif

/** swap two instances of a given type */
#define SWAP(_a, _b, _type)                     \
  do {                                \
  _type  _tmp;                    \
  _tmp = _a; _a = _b; _b = _tmp;  \
  } while(0)


/** a handful of bytes */
#define MAX_LINE        1024
/** a handful of bytes */
#define MAX_WORD        80

/** suppress unused parameters */
#define UNUSED(x)       (void)(x)

/** safe pointer dereferncing */
#define SAFE_ASSIGN(ptr, val)   if (ptr) { *ptr = val; }


/** public variable: program name for diagnostics */
global const char*      progname;


/* exported functions */

/** get basename (strip path) of a given filename                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                
    return a char pointer after the last occurance of '/',                                                                                                                                                                                                                      
    or to the beginning of the input string.                                                                                                                                                                                                                                    
    the function does not modify the input string.                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                
    \param pathname  input string                                                                                                                                                                                                                                               
*/
const char*  mybasename( const char* pathname );


/** strip path (get basename) and suffix (if given) from filenames                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                
    return a char pointer after the last occurance of '/'                                                                                                                                                                                                                       
    if a file extension was given (ie. it is not NULL) it is ripped                                                                                                                                                                                                             
    from the end of the file. When the extension does not match with                                                                                                                                                                                                            
    the actual extension of the filename, it is left unchanged.                                                                                                                                                                                                                 
    mybasenamet() modifies the string (writing a terminating character                                                                                                                                                                                                          
    to the place of the dot starting the extension) if the suffix is                                                                                                                                                                                                            
    found. (see also mybasename().)                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                
    \param filename  input string                                                                                                                                                                                                                                               
    \param extension  optional extension (may be NULL)                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                
const char* mybasenamet( const char* filename, const char* extension );                                                                                                                                                                                                         
*/

/** print version number of the program                                                                                                                                                                                                                                         
                                                                                                                                                                                                                                                                                
    print the version number coded in the source file (usually the                                                                                                                                                                                                              
    CVS Revision tag). The function does not return, its exit code                                                                                                                                                                                                              
    is EXIT_SUCCESS.                                                                                                                                                                                                                                                            
*/
//#ifdef __GNUC__                                                                                                                                                                                                                                                               
//void  version(void) __attribute__ ((noreturn));                                                                                                                                                                                                                               
//#else                                                                                                                                                                                                                                                                         
//void  version(void);                                                                                                                                                                                                                                                          
//#endif                                                                                                                                                                                                                                                                        


/** return version number of the program                                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                
    return the version number coded in the source file (usually the                                                                                                                                                                                                             
    CVS Revision tag).                                                                                                                                                                                                                                                          
*/
//const char* get_version();                                                                                                                                                                                                                                                    

// string concatenation
char *concat(const char *s1, const char *s2);

/** returns the first non-space character from the string or NULL if                                                                                                                                                                                                            
    there is none                                                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                
    empty_string() does not modify the original string.                                                                                                                                                                                                                         
    \param line  input string                                                                                                                                                                                                                                                   
    \todo  empty_string() crashes if the input string is NULL                                                                                                                                                                                                                   
*/
const char* empty_string( const char* line );

/** remove white space from the end of a string */
char*       trim_right( char* line );


/** print an array of strings to the specified file stream                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                                
    print each string (plus a newline) from the textarray until                                                                                                                                                                                                                 
    NULL is encountered.                                                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                
    \param file  output file stream                                                                                                                                                                                                                                             
    \param textarray  array of strings terminated by NULL                                                                                                                                                                                                                       
*/
void        display_longtext( stream file, const char** textarray );


/** change string into all upper case                                                                                                                                                                                                                                           
                                                                                                                                                                                                                                                                                
    \param  string  input string                                                                                                                                                                                                                                                
*/
void        change2upper( char* string );


/** change string into all lower case                                                                                                                                                                                                                                           
                                                                                                                                                                                                                                                                                
    \param  string  input string                                                                                                                                                                                                                                                
*/
void        change2lower( char* string );


/** find the given column in a string                                                                                                                                                                                                                                           
                                                                                                                                                                                                                                                                                
    search for the \a col column (C syntax: first column is 0)                                                                                                                                                                                                                  
    in the string \a line, return a pointer to it or NULL if not found.                                                                                                                                                                                                         
    returns the length of the column in the \a len variable (or 0 on                                                                                                                                                                                                            
    error). By default columns are separated by the space, tab, newline                                                                                                                                                                                                         
    or end of string characters, but these behaviour can be changed by                                                                                                                                                                                                          
    specifying other separators as strings.                                                                                                                                                                                                                                     
                                                                                                                                                                                                                                                                                
    \param line  input string                                                                                                                                                                                                                                                   
    \param col   number of column to search for (C syntax)                                                                                                                                                                                                                      
    \param separator  column separators (may be NULL)                                                                                                                                                                                                                           
    \param len  returned: length of the column                                                                                                                                                                                                                                  
*/
const char*     colfind(
                        const char*     line,
                        int             col,
                        const char*     separator,
                    int*            len
                        );

/** paste columns from line into an other string                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                                
    find a given column in \a line (see colfind()), and copy it into                                                                                                                                                                                                            
    \a column. the column number is given by the C synthax (0 is first).                                                                                                                                                                                                        
    Take care: \a column must be long enough to hold the entire text                                                                                                                                                                                                            
    copied into it!!!!                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                
    \param column  returned: the column from the \a line                                                                                                                                                                                                                        
    \param line    input string                                                                                                                                                                                                                                                 
    \param col     number of column (C synthax)                                                                                                                                                                                                                                 
    \param separator  column separators (may be NULL) (see colfind())                                                                                                                                                                                                           
*/
bool            colcut(
                       char*           column,
		       const char*     line,
                       int             col,
                    const char*     separator
                       );


/** convert the given column from a line into a floating point number                                                                                                                                                                                                           
                                                                                                                                                                                                                                                                                
    Use colcut() to read the string from the given column to turn                                                                                                                                                                                                               
    it into a floating point number by calling mystr2d(). Returns                                                                                                                                                                                                               
    FALSE on error.                                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                
    Take care: an internal buffer of MAX_LINE is used instead of                                                                                                                                                                                                                
    dynamically allocating memory. Possible buffer overflow.                                                                                                                                                                                                                    
                                                                                                                                                                                                                                                                                
    \todo fix buffer overflow possibility.                                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                                
    \param  line  input line                                                                                                                                                                                                                                                    
    \param  col   the column to be read (0 is first)                                                                                                                                                                                                                            
    \param  val   returned: the converted value                                                                                                                                                                                                                                 
*/
bool  getd_fromcol( const char* line, int col, real* val );


/** read integers from a string separated by specified special characters                                                                                                                                                                                                       
                                                                                                                                                                                                                                                                                
    get_cols will return an array from integers extracted from the                                                                                                                                                                                                              
    given string. When no field separator specified (\a sep is NULL)                                                                                                                                                                                                            
    get_cols will presume, that the list is coma separated. Returns                                                                                                                                                                                                             
    NULL on error.                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                
    get_cols modifies its input argument \a columns!                                                                                                                                                                                                                            
                                                                                                                                                                                                                                                                                
    \param columns  string with the integers                                                                                                                                                                                                                                    
    \param sep      string with the separator characters. Set to coma, if                                                                                                                                                                                                       
    its value is NULL                                                                                                                                                                                                                                                           
    \param length   returned: the length of the return array                                                                                                                                                                                                                    
*/
int*  get_cols( char* columns, const char* sep, int* length );


/** convert a string to into real number                                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                
    wrapper function to strtod(). mystr2d() does not modify the original                                                                                                                                                                                                        
    string.                                                                                                                                                                                                                                                                     
                                                                                                                                                                                                                                                                                
    \param  string  the number as a string                                                                                                                                                                                                                                      
    \param  value  returned: the converted value                                                                                                                                                                                                                                
*/
bool  mystr2d( const char* string, real* value );

/** convert a string to integer                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                
    wrapper function to strtol(). mystr2i() does not modify the original                                                                                                                                                                                                        
    string.                                                                                                                                                                                                                                                                     
                                                                                                                                                                                                                                                                                
    \param  string  the number as a string                                                                                                                                                                                                                                      
    \param  value  returned: the converted value                                                                                                                                                                                                                                
*/
bool  mystr2i( const char* text, int* value );


/** redirect error messages to a given stream                                                                                                                                                                                                                                   
                                                                                                                                                                                                                                                                                
    set the output stream used by err_print() and err_ret() to differ                                                                                                                                                                                                           
    from stderr. if its argument is either NULL or stderr, the error                                                                                                                                                                                                            
    stream will be reset to stderr.                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                
    \param errout  stream to print error messages                                                                                                                                                                                                                               
*/
void  set_errstream(stream errout);


/** print an error message and exit with \a errcode        

    print an error message (see set_errstream()) and the message from                                                                                                                                                                                                           
    strerror() if the errno indicates any error. this function does not return.                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                
    \param errcode  returns \a errcode to the shell                                                                                                                                                                                                                             
    \param errmessage  variable length string arguments                                                                                                                                                                                                                         
                                                                                                                                                                                                                                                                                
    \todo variable length arguments are not supported in C99                                                                                                                                                                                                                    
*/
#ifdef __GNUC__

void    err_ret(
                int errcode, const char* errmessage, ...
                ) __attribute__ (( noreturn ));

void    err_ret_failure(
                        const char* errmessage, ...
                        ) __attribute__ (( noreturn ));
#else

void    err_ret(int errcode, const char* errmessage, ...);

void    err_ret_failure( const char* errmessage, ... );

#endif

/** print message to stderr                                                                                                                                                                                                                                                     
                                                                                                                                                                                                                                                                                
    print an error message (see set_errstream()) and the message from                                                                                                                                                                                                           
    strerror() if the errno indicates any error. (see err_ret())                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                                
    \param errmessage  variable length string arguments                                                                                                                                                                                                                         
                                                                                                                                                                                                                                                                                
    \todo variable length arguments are not supported in C99                                                                                                                                                                                                                    
*/
void    err_print( const char* errmessage, ... );

/* print message with program name to stderr */
void    err_print_msg( const char* fmt, ... );

void    err_puts_msg( const char* msg );

/** append newline to stdout */
void    endl();

/** append newline to stream */
void    fendl( stream out );

/** safly close a stream */
int     fsafeclose( FILE* fstream );

#endif /* UTILS_H__ */

