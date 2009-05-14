#include <stdio.h>
#include <stdlib.h>
#include "main.h"

int example_Rand(void)
{
#ifdef EXAMPLE_RAND
  int w=10;
  int i=0;
  int j=0;
  int** arr=NULL;
  double f;

  arr = (int**) malloc(w*sizeof(int*));
  if( arr == NULL )
    printf("Insufficient memory\n");

  for(i=0; i<w; i++)
  {
    arr[i] = (int*) malloc(w*sizeof(int));
    if( arr[i] == NULL )
      printf("Insufficient memory\n");
  }

  for(i=0; i<w; i++)
  {
    for(j=0; j<w; j++)
    {
      f = 10.0*rand() / RAND_MAX;
      arr[i][j] = (int)f;
    }
  }

  for(i=0; i<w; i++)
  {
    for(j=0; j<w; j++)
      printf("arr[%d][%d]=%d ", i, j, arr[i][j]);
    printf("\n");
  }

#endif
  return 0;
}




