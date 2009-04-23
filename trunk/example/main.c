#include <stdio.h>
#include "main.h"

int inargc;
char* inargv[255];

int main(int argc, char** args){
	int i;
	inargc = argc;
    if (argc >= 255)
    {
        printf("Too many input arguments.\n");
        exit(0);
    }
	for (i=0; i<inargc; i++)
	{
		inargv[i] = args[i];
	}
	
	example_MPEG2Parser();
	example_DebugNCompilation();
	example_Macros();
	example_VariableArgFunctions();
	example_TypeConversion();
	example_MemcpyTest();
	example_ParseIndex();
	example_Task();
	example_S64();
/* test*/
	return 0;
}   
