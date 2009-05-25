#include <stdio.h>
#include <time.h>
#include "main.h"

#define YEAR ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
+ (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

#define MONTH (__DATE__ [2] == 'n' ? 1 \
: __DATE__ [2] == 'b' ? 2 \
: __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
: __DATE__ [2] == 'y' ? 5 \
: __DATE__ [2] == 'n' ? 6 \
: __DATE__ [2] == 'l' ? 7 \
: __DATE__ [2] == 'g' ? 8 \
: __DATE__ [2] == 'p' ? 9 \
: __DATE__ [2] == 't' ? 10 \
: __DATE__ [2] == 'v' ? 11 : 12)

#define DAY ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
+ (__DATE__ [5] - '0'))

#define HOUR ( ( (__TIME__[0]-'0')*10 + (__TIME__[1]-'0')) )
#define MIN  ( ( (__TIME__[3]-'0')*10 + (__TIME__[4]-'0')) )
#define SECOND ( ( (__TIME__[6]-'0')*10 + (__TIME__[7]-'0')) )

int example_Time(void)
{
  struct tm *ptr;
  time_t t1;
  char str[100];

#ifdef EXAMPLE_TIME

  printf("Time of compilation is %s %s\n", __DATE__, __TIME__);
  printf("Time of compilation in integers: %d-%d-%d %d:%d:%d\n", YEAR, MONTH, DAY, HOUR, MIN, SECOND);

  t1 = time(NULL);
  ptr = localtime(&t1);
  strftime(str, 100, "%F %T.", ptr);
  printf("It's now %s\n", str);

  printf("ctime is: %s", ctime(&t1));
  printf("asctime is: %s", asctime(ptr));

#endif

  return 0;
}
