/* utils.c : misc functions and definitions (domsa@konkoly.hu (2002)) */
/* utils.c,v 3.2 2003/05/09 23:23:11 domsa Exp */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>

#include "utils.h"


/** local variables **/

/* error output stream */
local stream  errout = NULL;

/* CVS version info */
//extern const char* Version;                                                                                                                                                                                                                                                   


/** local functions **/

/* print error message */
local void  errprint(int /*errno*/, const char* /*fmt*/, va_list);


/** functions exported **/

/* strip path from filenames */
const char* mybasename( const char* s )
{
  const char* p;

  if(s == NULL)
    return  NULL;

  if((p = strrchr(s, '/')) != NULL)
    p++;
  else
    p = s;

  return  p;
}  /* endof mybasename() */                                                                                                                                                                                                                                                

// string concatenation
char *concat(const char *s1, const char *s2)
{
  char *result;

  result = malloc(strlen(s1) + strlen(s2) + 1);
  if (result == NULL)
    {
      printf("Error: malloc failed in concat\n");
      exit(EXIT_FAILURE);
    }
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

/* returns the 1st non-space character or NULL if none found */

const char* empty_string( const char* line )
{
  const char *p = line;
  if ( p == NULL ) {
    return NULL;
  }

  while ( *p != '\0' ) {
    if ( isspace( *p ) ) {
      ++p;
    }
    else {
      return p;
    }
  }

  return NULL;
}

char* trim_right( char* line )
{
  if ( line && *line ) {
    char* ptr = line + strlen( line ) - 1;
    while ( ptr >= line && isspace( *ptr ) ) {
      *ptr = '\0';
      --ptr;
    }
  }

  return line;
}

/* print char arrays to the given file descriptor */
void  display_longtext( stream fp, const char** text )
{
  if(fp == NULL || text == NULL)
    return;

  while(*text != NULL)
    fprintf(fp, "%s\n", *text++);

  return;
}  /* endof display_longtext() */

/* change text into all uppercase */
void  change2upper(char *text)
{
  if(text != NULL)
    {
      int   length = strlen(text);

      for(; length--; text++)
        if(islower(*text))
          *text = toupper(*text);
    }

  return;
}  /* endof change2upper() */


/* change text into all lowercase */
void  change2lower(char *text)
{
  if(text != NULL)
    {
      int   length = strlen(text);

      for(; length--; text++)
        if(isupper(*text))
          *text = tolower(*text);
    }

  return;
}  /* endof change2lower() */

/* find "col"th column (C synthax: 0 is the first column) */
const char* colfind(
                    const char* line,
                    int         col,
                    const char* separator,
    int*        len
                    )
{
  static const char mysep[] = " \t\n\0";
  int          ii;
  const char*  sep;
  const char   *word, *endp;

  *len = 0;

  if(line == NULL || col < 0)
    return  NULL;

  sep = separator == NULL ? mysep : separator;

  /* find column */
  word = line + strspn(line, sep);
  for(ii = 0; word < (endp = word + strcspn(word, sep)) && ii++ < col;
      word = endp + strspn(endp, sep))
    ;

  if(word == endp)
    return  NULL;

  *len = endp - word;

  return  word;
}  /* endof colfind() */


/* colcut(): cut "col"th column (C synthax: 0 is 1st column) from line  */
/* Columns are deliminated with characters in "separator". If it's NULL */
/* colcut() defaults to white spaces and tabs. The result is written to */
/* "column". **Be careful to allocate enough space to hold column!!**   */
bool colcut(
            char*       column,
            const char* line,
            int         col,
    const char* separator
            )
{
  const char  *word, *end;
  int   len;

  if((word = colfind(line, col, separator, &len)) == NULL)
    return false;

  end = word + len;

  while(isspace(*word))
    word++;

  while(--end >= word && isspace(*end))
    ;

  len = end - word + 1;

  strncpy(column, word, (size_t) len);
  *(column + len) = '\0';

  return true;
}  /* endof colcut() */


/* read a floating point number from a given column of the input line */
bool getd_fromcol( const char* line, int col, real* val )
{
  char column[MAX_LINE];

  if ( !colcut( column, line, col, NULL ) )
    return false;

  /* convert string to double */
  return mystr2d( column, val );
}  /* endof getd_fromcol() */

/* get integer numbers from a string separated by "sep" */
int* get_cols( char* columns, const char* sep, int* length )
{
  const char  *s;
  int   number;
  int   ncols = 0;
  int   *newcols, *cols = NULL;

  *length = 0;
  ncols   = 0;

  if(sep == NULL)
    sep = ",";

  s = strtok(columns, sep);

  do
    {
      if(mystr2i(s, &number) == false
         || (newcols = realloc((void *) cols, ++ncols*sizeof(*cols))) == NULL)
        {
          free(cols);
          return  NULL;
        }

      cols = newcols;
      cols[ncols - 1] = number;
    } while((s = strtok(NULL, sep)));


  *length = ncols;
  return  cols;
}  /* endof get_cols() */


/* convert a string to real (wrapper to strtod() ) */
bool mystr2d( const char* text, real* val )
{
  if ( text == NULL || !*text || val == NULL )
    return  false;

  char* endptr;
  real num;
  num = strtod( text, &endptr );
  if ( !*endptr ) {
    *val = num;
    return true;
  }

  return false;
}  /* endof mystr2d() */


/* convert a string to int (wrapper to strtol() ) */
bool mystr2i( const char* text, int* val )
{
  if ( text == NULL || !*text || val == NULL )
    return false;

  char* endptr;
  int num;
  num = ( int )strtol( text, &endptr, 10 );

  if ( !*endptr ) {
    *val = num;
    return true;
  }

  return false;
}  /* endof mystr2i() */

/* redirect error messages to given stream instead of stderr. */
void  set_errstream(stream newerr)
{
  errout = newerr == NULL ? stderr : newerr;
}  /* endof set_errstream() */


/* print error message and die with "errcode" */
void  err_ret(int errcode, const char* fmt, ...)
{
  int errnosave = errno;
  va_list  ap;

  va_start(ap, fmt);
  errprint(errnosave, fmt, ap);
  va_end(ap);

  exit(errcode == 0 ? errnosave : errcode);
}  /* endof err_ret() */


void err_ret_failure( const char* fmt, ...)
{
  fprintf( stderr, "%s: ", progname );

  va_list ap;
  va_start( ap, fmt );
  vfprintf( stderr, fmt, ap );
  va_end( ap );

  fendl( stderr );

  exit( EXIT_FAILURE );
}

/* print error message */
void  err_print( const char* fmt, ...)
{
  int      errnosave = errno;
  va_list  ap;

  va_start(ap, fmt);
  errprint(errnosave, fmt, ap);
  va_end(ap);

  return;
}  /* endof err_print() */


/* print error message */
local void  errprint(int errnosave, const char* fmt, va_list ap)
{
  char  buffer[MAX_LINE+1];

  if(errout == NULL)
    errout = stderr;

  (void) vsnprintf(buffer, MAX_LINE, fmt, ap);
  if(errnosave)
    {
      int  len = strlen(buffer);
      (void) snprintf(buffer+len, MAX_LINE-len, ": %s", strerror(errnosave));
    }

  (void) fputs(buffer, errout);
  (void) fputc('\n', errout);
  (void) fflush(NULL);

  return;
}  /* endof errprint() */


void err_print_msg( const char* fmt, ... )
{
  fprintf( stderr, "%s: ", progname );

  va_list ap;
  va_start( ap, fmt );
  vfprintf( stderr, fmt, ap );
  va_end( ap );

  fendl( stderr );
}

void err_puts_msg( const char* msg )
{
  fputs( progname, stderr );
  fputs( ": ", stderr );
  fputs( msg, stderr );
  fendl( stderr );
}

void endl()
{
  fendl( stdout );
}

void fendl( stream out )
{
  fputc( '\n', out );
}

int fsafeclose( FILE* fstream )
{
  if (
            fstream
        &&  fstream != stdin
        &&  fstream != stdout
        &&  fstream != stderr
      ) {
    return fclose( fstream );
  }

  return 0;
}

/*int main(void)          
{
	return 0;

}*/

