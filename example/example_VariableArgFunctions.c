#include <stdio.h>
#include <stdarg.h>
#include "main.h"

int Display_WithFormator(char* str, ...)
{
	va_list list;
	va_start(list, str);
	
	fprintf(stdout, "%s\n", __FUNCTION__);	
	vfprintf(stdout, str, list);
	
	fprintf(stdout, "\n");
	va_end(list);
	
	return 0;
}

int Display_WithoutFormator(char* str, ...)
{
	va_list list;
	va_start(list, str);

	fprintf(stdout, "%s\n", __FUNCTION__);
	fprintf(stdout, "%s", str);	
	while(1)
	{
		char* nstr = va_arg(list, char *);
		if (nstr == NULL)
			break;
		fprintf(stdout, "%s", nstr);
	}

	fprintf(stdout, "\n");	
	va_end(list);
	return 0;
}

int example_VariableArgFunctions(void)
{
#ifdef EXAMPLE_VARIABLEARGFUNCTIONS
	Display_WithFormator("%s%s%s", "abc ", "def ", "ghi ");
	Display_WithoutFormator("abc ", "def ", NULL);
#endif
	return 0;
}
