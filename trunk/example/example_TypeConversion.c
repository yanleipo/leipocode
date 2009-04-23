#include "main.h"

union {
   int i[2];
   double d;
} un;

#ifdef RUN_ON_SH4
void set_denorm(void)
{
        extern unsigned int __fpscr_values[2];
        unsigned long fpscr;
        __fpscr_values[0] |= 0x40000;
        __fpscr_values[1] |= 0x40000;
        asm volatile ("sts fpscr, %0" : "=r" (fpscr));
        fpscr |= 0x40000;
        asm volatile ("lds %0, fpscr" : : "r" (fpscr));
}
#endif

void show_Info(void)
{
   printf("-------------------------- Floating Point Standard ------------------------------\n");
   printf("* sign(1bit)   exponent(8bits)    fraction(23bits)                              *\n");
   printf("* |                     |                                                       *\n");
   printf("* 1 1 0 0 | 0 0 1 0 | 1 1 1 0 | 1 1 0 1 | 0 1 0 0 | 0 0 0 0 | 0 0 0 0 | 0 0 0 0 *\n");
   printf("* f = s * 2^e * m                                                               *\n");
   printf("* where                                                                         *\n");
   printf("* s = +1; when the sign bit is 0                                                *\n");
   printf("* s = -1; when the sigb bit is 1                                                *\n");
   printf("* e = Exp - 127; the exponent is stored with 127 added to it, \"biased with 127\" *\n");
   printf("* m = 1.fraction in binary; therefore, 1 <= m < 2                               *\n");
   printf("* Hence,                                                                        *\n");
   printf("* f = -1 * 2^6 * (1 + 2^-1 + 2^-2 + 2^-4 + 2^-5 + 2^-7 + 2^-9) = -118.625       *\n\n");
   printf("-------------------------- Double precision Standard ----------------------------\n");
   printf("* sign(1bit)   exponent(11bits)    fraction(52bits)                             *\n");
   printf("* Double is biased with 1023                                                    *\n");
   printf("---------------------------------------------------------------------------------\n");
   printf("Size of int    : %d Bytes\n", sizeof(int));
   printf("Size of float  : %d Bytes\n", sizeof(float));
   printf("Size of double : %d Bytes\n\n", sizeof(double));
}

int example_TypeConversion(void)
{
#ifdef EXAMPLE_TYPECONVERSION
   float f;
   double dnum;
   int i=10;

#ifdef RUN_ON_SH4   
   set_denorm();
#endif
	
   show_Info();

   un.i[1] = 0x405DA800; /* 118.625 in double */
   un.i[0] = 0x0;
   f = (float) un.d;
   printf("(double) %.8x|%.8x -> (float) %.8x %f (within range)\n", un.i[1], un.i[0], f);

   un.i[1] = 0x7FEDA800;
   un.i[0] = 0x0;
   f = (float) un.d;
   printf("(double) %.8x|%.8x -> (float) %.8x %f (too large. Overflow exception?)\n", un.i[1], un.i[0], f);

   f = (float) i;
   printf("(int)    %.8x          -> (float) %.8x %f (Not simple memory translation)\n", i, f);
   
   un.i[1] = 0x0;
   un.i[0] = 0x1;
   f = (float) un.d;
   printf("(double) %.8x|%.8x -> (float) %.8x %f (too small. Underflow exception?)\n", un.i[1], un.i[0], f);

   un.i[1] = 0x000100000;
   un.i[0] = 0x0;
   f = (float) un.d;
   printf("(double) %.8x|%.8x -> (float) %.8x %f (too small. Underflow exception?)\n", un.i[1], un.i[0], f, f);
#endif
   return 0;
}
