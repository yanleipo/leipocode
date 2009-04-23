#include "list.h"
#include "log.h"

/*
** Return first element in the list
*/
void*          list_head( List *L )
{
  if( L->head == NULL )
  {
    return ( NULL );
  }

  return( L->head->data );
}

/*
** Return last element in the list
*/
void*          list_tail( List *L )
{
  if( L->tail == NULL )
  {
    return( NULL );
  }

  return L->tail->data;
}

/*
** Return the number of elements in the list
*/
int            list_size( List *L )
{
  return( L->size );
}

/*
** Find the node position based on data
*/
static Node*     list_findPos( List* L, void* data )
{
  Node* pos;

  if( data == NULL )
  {
    ERROR( "NULL can not be in the list.");
    return( NULL );
  }

  pos = L->head;
  while( pos != NULL && pos->data != data)
  {
    pos = pos->next;
  }

  return pos;
}

/*
** Return the next element of dataRef in the list
*/
void*           list_nextData( List *L, void* dataRef )
{
  Node* pos = NULL;
 
  pos = list_findPos( L, dataRef );

  if( pos == NULL || pos == L->tail)
  {
    return( NULL );
  }
  
  return( pos->next->data );
}

/*
/* Return the previous element of dataRef in the list
*/
void*           list_prevData( List *L, void* dataRef )
{
  Node* pos = NULL;
 
  pos = list_findPos( L, dataRef );

  if( pos == NULL || pos == L->head )
  {
    return( NULL );
  }
  
  return( pos->prev->data );
}



/*
** Check if the data is in the list based on the pointer (not content)
*/
int             list_hasMember( List *L, void* data )
{
  Node* pos = NULL;

  pos = list_findPos( L, data );
  if( pos == NULL )
  {
    return( FALSE );
  }

  return( TRUE );
}




/*
** Insert the data into the list after dataRef
*/
ListError         list_insert( List *L, void* data, void* dataRef )
{
  Node* newNode = NULL;
  Node* pos = NULL;

  /* Check if the data to be inserted is already in the list */
  /* ------------------------------------------------------- */
  if( list_hasMember( L, data ) )
  {
    ERROR( "List already has the member." );
    return( LIST_ERROR_ELEMENT_ALREADY_IN_LIST );
  }

  /* Allocate and initialize new node */
  /* -------------------------------- */
  newNode = ( Node* )malloc( sizeof( Node ) );
  if( newNode == NULL )
  {
    ERROR( "Insufficient Memory." );
    return( LIST_ERROR_INSUFFICIENT_MEMORY );
  }
  newNode->data = data;
  newNode->next = NULL;
  newNode->prev = NULL;

  /* If the list is empty, directly assign the node to the list */
  /* ---------------------------------------------------------- */
  if( L->size == 0 )
  {
    L->head = newNode;
    L->tail = newNode;
    L->size = 1;
    return( LIST_ERROR_OK );
  }

  /* Check if the reference data is in the list */
  /* ------------------------------------------ */
  pos = list_findPos( L, dataRef );
  if( pos == NULL )
  {
    ERROR( "The reference data is not in the list. Dont know where to insert.");
    free( newNode );
    return( LIST_ERROR_ELEMENT_NOT_IN_LIST );
  }

  if( pos == L->tail )
  {
    pos->next = newNode;
    newNode->prev = pos;
    L->tail = newNode;
  }
  else
  {
    newNode->next = pos->next;
    newNode->prev = pos;
    pos->next->prev = newNode;
    pos->next = newNode;
  }
  L->size++;

  return( LIST_ERROR_OK );
}

/*
** Append the data to the end of the list
*/
ListError       list_append( List *L, void* data )
{
  if( L->tail == NULL )
    return( list_insert( L, data, NULL ) );
  else
    return( list_insert( L, data, list_tail( L ) ) );
}

/*
** Delete the data from the list
*/
ListError       list_remove( List *L, void* data )
{
  Node* pos = NULL;

  pos = list_findPos( L, data );
  if( pos == NULL )
  {
    ERROR( "Unable to find data from the list.");
    return( LIST_ERROR_ELEMENT_NOT_IN_LIST );
  }

  if( pos == L->head && pos == L->tail )
  {
    L->head = NULL;
    L->tail = NULL;
    L->size = 0;
    free(pos);
    return( LIST_ERROR_OK );  
  }

  if( pos == L->head )
  {
    L->head = pos->next;
    pos->next->prev = NULL;
  } else if( pos == L->tail )
  {
    L->tail = pos->prev;
    pos->prev->next = NULL;
  } else
  {
    pos->next->prev = pos->prev;
    pos->prev->next = pos->next;
  }

  L->size--;
  free(pos);

  return( LIST_ERROR_OK );  
}

/*
** Init list
*/
ListError      list_init( List *L )
{
  L->head = NULL;
  L->tail = NULL;
  L->size = 0;

  return( LIST_ERROR_OK );
}

/*
** Delete all the data in the list
*/
ListError      list_delete( List *L )
{
  Node* pos;

  pos = L->head;

  while( pos != NULL )
  {
    list_remove( L, pos->data );
    pos = L->head;
  }

  return( LIST_ERROR_OK ); 
}

