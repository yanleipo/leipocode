#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "log.h"

typedef struct member_s
{
  int         id;
  char        name[25];
} member_t;

void
PrintList( const List L )
{
  member_t *mem_p;

  mem_p = ( member_t* ) list_head( &L );

  if( L.size == 0 )
    printf( "Empty\n" );
  else
  {
    while( mem_p != NULL )
    {
      printf( "(%d,%s) ", mem_p->id, mem_p->name );
      mem_p = list_nextData( &L, mem_p );
    }
    printf( "\n" );
  }
}

main( )
{
  int i;
  List L={0};
  member_t member[20];
  member_t *mem_p;

  /* Init member datas */
  for( i = 0; i < 20; i++ )
  {
    member[i].id=i;
    sprintf(member[i].name, "mem%d\0", 20-i);
  }

  /* Initializing the list is necessary */
  list_init( &L );

  /* Insert into the list */
  for( i = 0; i < 20; i+=1 )
  {
    list_append( &L, &member[i] );	
    PrintList( L );
  }

  for( i = 0; i < 20; i+= 2 )
    list_remove( &L, &member[i] );

  for( i = 0; i < 20; i++ )
    if( ( i % 2 == 0 ) && ( list_hasMember( &L, &member[i] ) == TRUE ) )
      LOG( "Find fails" );

  LOG( "Finished deletions. L.size=%d\n", L.size );

  PrintList( L );
  list_delete( &L );

  PrintList( L );
  return 0;
}
