#include <stdio.h>

void help(void)
{
  printf(" * Usage: ./output.exe x\n");
  printf(" * \n");
  printf(" * x is arbitary number.\n");
  printf(" * \n");
  printf(" * This program displays the number in dec, hex and binary mode\n");
}

int main(int argc, char* argv[])
{

  int num;
  int* num_bin;
  int len, i;

  if( argc < 2 )
  {
    help();
    return 0;
  }

  num = atoi( argv[1] );

  printf("num in dec=0d%d\n", num);
  printf("num in hex=0x%x\n", num);

  /* Convert the number into binary mode */
  len = sizeof(num) * 8;
  num_bin = (int*)malloc(len*sizeof(int));
  if(num_bin == NULL)
  {
    printf("Insufficient mem\n");
    return 0;
  }

  for(i=len-1; i>=0; i--)
    num_bin[i] = (num>>i) & 1;

  printf("num in bin=0b", num);
  for(i=len-1; i>=0; i--) printf("%d", num_bin[i]);
  printf("\n");

  return 0;

}
