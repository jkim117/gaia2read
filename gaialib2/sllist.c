/* sllist.c : single-linked list routines */

#include "sllist.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>


/* add an item to sl-list */

local sllist* alloc_item( const void* item, size_t size )
{
  sllist* newitem = NULL;

  if ( !item || !size ) {
    return NULL;
  }

  newitem = malloc( sizeof *newitem );
  if ( !newitem ) {
    err_print( "%s: cannot allocate enough memory", __FUNCTION__, size );
    return NULL;
  }

  newitem->size = size;
  newitem->data = malloc( size );
  if ( newitem->data == NULL ) {
    err_print( "%s: cannot allocate enough memory", __FUNCTION__, size );
    return NULL;
  }

  memcpy( newitem->data, item, size );
  return newitem;
}

sllist* sladd( sllist** list, const void* item, size_t size )
{
  sllist* newitem = alloc_item( item, size );

  if ( newitem == NULL ) {
    return NULL;
  }

  if ( *list == NULL ) {
    newitem->next = NULL;
    *list = newitem;
  }
  else {
    newitem->next = (*list)->next;
    (*list)->next = newitem;
  }

  return newitem;
}

sllist* slappend( sllist** list, const void* item, size_t size )
{
  sllist* newitem = alloc_item( item, size );

  if ( newitem == NULL ) {
    return NULL;
  }

  if ( *list == NULL ) {
    newitem->next = NULL;
    *list = newitem;
  }
  else {
    sllist* last = sllast( *list );
    last->next = newitem;
    newitem->next = NULL;
  }

  return newitem;
}

/* delete current item from list */

sllist  *sldel(sllist *item)
{
  sllist  *next = NULL;

  if(item != NULL)
    {
      next = item->next;

      if(item->data != NULL)
        free(item->data);

      free(item);
    }

  return next;
}/* endof sldel() */

/* destroy the whole sl-list */

void sldestroy(sllist **list)
{
  sllist  *next = *list;

  if(next != NULL)
    {
    do
      {
        next = sldel(next);
      } while(next != NULL);
    }

  return;
}/* endof sldestroy() */


/* walk a list, passing each data field to function "func" */
/* returns true if element "found", false if end of list reached */

bool  slwalk(sllist *list, listiter *func, void *args)
{
  sllist  *this;
  bool    ret = false;/* loop terminates if ret is true */

  for(this = list; ret == false && this != NULL; this = this->next)
    ret = func(this->data, args);/* item's data and supplied args */

  return ret;
}/* endof slwalk() */


sllist* slconcatentate( sllist* lhs, sllist* rhs )
{
  if ( !lhs )
    return rhs;
  if ( !rhs )
    return lhs;

  sllist* last = sllast( lhs );
  last->next = rhs;

  return lhs;
}

sllist* sllast( sllist* lst )
{
  if ( !lst )
    return NULL;

  sllist* last = lst;
  for ( ; last->next; last = last->next )
    ;

  return last;
}


#if 0
/* few print functions for the most common data-types */
bool  slprint_int( void* text, void* args )
{
  printf("%d\n", *((int *) text));
  return  false;
}  /* endof slprint_int() */

bool  slprint_real( void* text, void* args )
{
  printf("%f\n", *((real *) text));
  return  false;
}  /* endof slprint_real() */

bool  slprint_str( void* text, void* args )
{
  printf("%s\n", (string) text);
  return  false;
}  /* endof slprint_real() */
#endif


#ifdef MAIN

/* a simple demonstration */

/* function of "listiter" */

bool  printitle(void *title, void *args)
{
  printf("%s: %s\n", (string) args, (string) title);
  return false;/* loop through list */
}

int main()
{
  char    *titles[] = { "G6H6", "Numerical Recepies", "IDL", NULL };
  char    text[]    = "Book title: ";
  sllist  *books = NULL;
  int     i;

  for(i = 0; titles[i] != NULL; i++)
    sladd(&books, (void *) titles[i], strlen(titles[i]) + 1);

  slwalk(books, printitle, (void *) text);

  sldestroy(&books);

  exit(0);
}

#endif /* MAIN */

