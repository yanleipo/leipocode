/* list.h
** Created on Sun Mar 01 2009
*/

#ifndef _LIST_H_
#define _LIST_H_

#include <stdio.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE  1

typedef enum ListError_t
{
  LIST_ERROR_OK = 0,
  LIST_ERROR_INSUFFICIENT_MEMORY,
  LIST_ERROR_ELEMENT_ALREADY_IN_LIST,
  LIST_ERROR_ELEMENT_NOT_IN_LIST
} ListError;
 
typedef struct Node_t
{
  void*            data;
  struct Node_t    *next;
  struct Node_t    *prev;
} Node;

typedef struct List_t
{
  int            size;
  struct Node_t* head;
  struct Node_t* tail;
} List;

/*
** Return first element in the list
*/
void*          list_head( List *L );

/*
** Return last element in the list
*/
void*          list_tail( List *L );

/*
** Return the number of elements in the list
*/
int            list_size( List *L );

/*
/* Return the next element of dataRef in the list
*/
void*          list_nextData( List *L, void* dataRef );

/*
/* Return the previous element of dataRef in the list
*/
void*          list_prevData( List *L, void* dataRef );

/*
** Check if the data is in the list based on the pointer (not content)
*/
int            list_hasMember( List *L, void* data );

/*
** Insert the data into the list after dataRef
*/
ListError      list_insert( List *L, void* data, void* dataRef );

/*
** Append the data to the end of the list
*/
ListError      list_append( List *L, void* data );

/*
** Delete the data from the list
*/
ListError      list_remove( List *L, void* data );

/*
** Init list
*/
ListError      list_init( List *L );

/*
** Delete all the data in the list
*/
ListError      list_delete( List *L );

#endif    /* _LIST_H */

