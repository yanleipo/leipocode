#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64

#include "main.h"
#include <fcntl.h>

#define DEBUG_MACROS
#define TESTP printf("line is: %d\n", __LINE__)

#ifdef DEBUG_MACROS
#define MACROS_DEBUG printf
#else
#define MACROS_DEBUG
#endif

int example_Macros(void)
{
#ifdef EXAMPLE_MACROS
	MACROS_DEBUG("Current file is %s\n", __FILE__);
	MACROS_DEBUG("Current function is %s\n", __FUNCTION__);
	MACROS_DEBUG("Current line is %d\n", __LINE__);
	MACROS_DEBUG("Current data is %s\n", __DATE__);
	MACROS_DEBUG("Current time is %s\n", __TIME__);
	MACROS_DEBUG("GCC version is %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
	MACROS_DEBUG("O_RDONLY: 0x%x\n", O_RDONLY);
	MACROS_DEBUG("O_WRONLY: 0x%x\n", O_WRONLY);
	MACROS_DEBUG("O_RDWR:   0x%x\n", O_RDWR);
	MACROS_DEBUG("O_ACCMODE:0x%x\n", O_ACCMODE);
	MACROS_DEBUG("O_CREAT:  0x%x\n", O_CREAT);
	MACROS_DEBUG("O_APPEND: 0x%x\n", O_APPEND);
	MACROS_DEBUG("O_LARGEFILE: 0x%x\n", O_LARGEFILE);
	TESTP;
	TESTP;
	TESTP;
#endif
	return 0;
}
