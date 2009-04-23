#include <stdio.h>
#include <sys/time.h>
#include "main.h"

// times() in normal Linux is 10ms precision.
// I recommend gettimeofday().

static struct timeval __tv;

void start_watch(void)
{
    printf("%s\n", __func__);
    gettimeofday(&__tv, NULL);
}

unsigned long stop_watch(char *msg)
{
    long usec;
    struct timeval tv2;  

    gettimeofday(&tv2, NULL);

    printf("%s\n", __func__);

    usec = tv2.tv_usec - __tv.tv_usec;

    if(usec < 0)
    {
        usec += 1000000;

        tv2.tv_sec--;
    }

    printf("%s: %ldsec %03ldmsec %03ldusec\n", msg, (long)(tv2.tv_sec - __tv.tv_sec), usec/1000, usec%1000);

    return ((tv2.tv_sec - __tv.tv_sec)*1000 + usec/1000);
}

void sync(void)
{

//  __asm__ volatile ("sync");

}

#define CHUNK_SZ   (960*540*4)

#define NR_CHUNK   2000

int example_MemcpyTest(void)

{
#ifdef EXAMPLE_MEMCPYTEST
    unsigned long elapsed;

    char *p1 = (char*)malloc(CHUNK_SZ);
    char *p2 = (char*)malloc(CHUNK_SZ);

    int i;

    memset(p1, 0x55, CHUNK_SZ); // need to alloc physically
    memset(p2, 0xaa, CHUNK_SZ);

    start_watch();

    for(i=0; i<NR_CHUNK; i++)
    {
        memcpy(p1, p2, CHUNK_SZ);
        sync();
    }

    elapsed = stop_watch("memcpy");
    printf("BandWidth %fMB/s\n", 1.0*NR_CHUNK*CHUNK_SZ/elapsed/1000/*ms*/);

    free(p2);
    free(p1);
#endif
    return 0;
}


