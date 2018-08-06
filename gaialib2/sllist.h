/* sllist.h : single-linked list routines */

#ifndef SLLIST_H__
#define SLLIST_H__

#include "utils.h"


/* list iterator function prototype */

typedef bool listiter( void*, void* );


/* single-linked list */

typedef struct sllist
{
  struct sllist*    next;
  void*             data;
  size_t            size;
}   sllist;


/* add an item to the sl-list, return new item */
sllist* sladd( sllist** list, const void* item, size_t size );
sllist* slappend( sllist** list, const void* item, size_t size );

/* delete the last item from the sl-list. ret previous item */
sllist* sldel(sllist * /*list*/);

/* destroy the whole list */
void    sldestroy(sllist ** /*list*/);

/* walk a list. walk terminates if function returns TRUE or end of list */
bool    slwalk(sllist * /*list*/, listiter * /*function*/, void * /*args*/);

sllist* slconcatentate( sllist* lhs, sllist* rhs );

sllist* sllast( sllist* lst );


#if 0
/* few print functions for the most common data-types */
bool  slprint_int(void *, void *);
bool  slprint_real(void *, void *);
bool  slprint_str(void *, void *);
#endif

#endif /* SLLIST_H__ */

