#include "main.h"

//#define DEBUG_DEBUGNCOMPILATION
#ifdef DEBUG_DEBUGNCOMPILATION
#define DEBUGNCOMPILATION_DEBUG printf
#else
#define DEBUGNCOMPILATION_DEBUG
#endif


#if defined(DEBUG_DEBUGNCOMPILATION) && !defined(EXAMPLE_DEBUGNCOMPILATION)
#error "Not allowed."
#endif

int example_DebugNCompilation(void)
{
#ifdef EXAMPLE_DEBUGNCOMPILATION
	int i=0;

        for (i=0; i< 10000; i++)
                DEBUGNCOMPILATION_DEBUG("i=%d  imod8=%d imod3=%d.\n", i++, i%7, i%3);

	#ifdef NUM_DEF
        printf("NUM_DEF: %d\n", NUM_DEF);
	#endif

	#ifdef STR_DEF
	printf("STR_DEF: %s\n", STR_DEF);
	#endif

	#if !defined(NUM_DEF) && !defined(STR_DEF)
	printf("Please try to add -DNUM_DEF=2 or -DSTR_DEF='\"abc\"' to the compilation flag.\n");
	#endif
#endif
        return 0;
}




