#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64

#include "main.h"

#ifndef DEFINED_U64
#define DEFINED_U64
typedef struct U64_s
{
    unsigned int LSW;
    unsigned int MSW;
}
U64;
#endif

/*The following S64 type is not truly a Signed number. You would have to use additional
  logic to use it alongwith the macro I64_IsNegative(A) provided below*/
#ifndef DEFINED_S64
#define DEFINED_S64
typedef U64 S64;
#endif

/*Value=A+B, where A & B is U64 type*/
#define I64_Add(A,B,Value)      { register long long T1,T2,Val; \
                                T1  = (long long)(A).MSW << 32 | (A).LSW; \
                                T2  = (long long)(B).MSW << 32 | (B).LSW; \
                                Val = T1 + T2; \
                                (Value).MSW = Val >> 32; \
                                (Value).LSW = (U32)Val; \
                                }

/*Value=A+B, where A is U64 type & B is 32-bit atmost*/
#define I64_AddLit(A,B,Value)   { register long long T1,Val; \
                                T1 = (long long)(A).MSW << 32 | (A).LSW; \
                                Val=T1+(B); \
                                (Value).MSW = Val >> 32; \
                                (Value).LSW = (U32)Val; \
                                }

/*A==B, A & B are U64 type*/
#define I64_IsEqual(A,B)                (((A).LSW == (B).LSW) && ((A).MSW == (B).MSW))

#define I64_GetValue(Value,Lower,Upper) ((Lower) = (Value).LSW, (Upper) = (Value).MSW)

/*A>=B, A & B are U64 type*/
#define I64_IsGreaterOrEqual(A,B)       ( ((A).MSW >  (B).MSW) || \
                                         (((A).MSW == (B).MSW) && ((A).LSW >= (B).LSW)))

/*A>B, A & B are U64 type*/
#define I64_IsGreaterThan(A,B)          ( ((A).MSW >  (B).MSW) || \
                                         (((A).MSW == (B).MSW) && ((A).LSW > (B).LSW)))

/*A<B, A & B are U64 type*/
#define I64_IsLessThan(A,B)             ( ((A).MSW <  (B).MSW) || \
                                         (((A).MSW == (B).MSW) && ((A).LSW < (B).LSW)))

/*A<=B, A & B are U64 type*/
#define I64_IsLessOrEqual(A,B)          ( ((A).MSW <  (B).MSW) || \
                                         (((A).MSW == (B).MSW) && ((A).LSW <= (B).LSW)))

#define I64_IsNegative(A)               ((A).MSW & 0X80000000)

/*A==0, A is U64 type*/
#define I64_IsZero(A)                   (((A).LSW == 0) && ((A).MSW == 0))

/*A!=B, A & B are U64 type*/
#define I64_AreNotEqual(A,B)            (((A).LSW != (B).LSW) || ((A).MSW != (B).MSW))

#define I64_SetValue(Lower,Upper,Value) ((Value).LSW = (Lower), (Value).MSW = (Upper))

/*Value=A-B, where A & B are U64 type*/
#define I64_Sub(A,B,Value)              ((Value).MSW  = (A).MSW - (B).MSW - (((A).LSW < (B).LSW)?1:0), \
                                         (Value).LSW  = (A).LSW - (B).LSW)

/*Value=A-B, where A is U64 type & B is 32-bit atmost*/
#define I64_SubLit(A,B,Value)           ((Value).MSW  = (A).MSW - (((A).LSW < (B))?1:0), \
                                         (Value).LSW  = (A).LSW - (B))

/*Value=A/B, where A is U64 type & B is 32-bit atmost*/
#define I64_DivLit(A,B,Value)           { register long long T1, Val;\
                                        T1 = (long long)(A).MSW << 32 | (A).LSW;\
                                        Val = T1/(B);\
                                        (Value).MSW = Val >> 32;\
                                        (Value).LSW = (U32)Val;\
                                        }

/*Value=A%B, where A is U64 type & B is 32-bit atmost*/
#define I64_ModLit(A,B,Value)           { register long long T1, Val;\
                                        T1 = (long long)(A).MSW << 32 | (A).LSW;\
                                        Val=T1%(B);\
                                        (Value).MSW = Val >> 32;\
                                        (Value).LSW = (U32)Val;\
                                        }

/*Value=A*B, where A & B are U64 type*/
#define I64_Mul(A,B,Value)              { register long long T1, T2, Val; \
                                        T1 = (long long)(A).MSW << 32 | (A).LSW; \
                                        T2 = (long long)(B).MSW << 32 | (B).LSW; \
                                        Val=T1*T2; \
                                        (Value).MSW = Val >> 32; \
                                        (Value).LSW = (U32)Val; \
                                        }

/*Value=A*B, where A is U64 type & B is 32-bit atmost*/
#define I64_MulLit(A,B,Value)           { register long long T1,Val; \
                                        T1 = (long long)(A).MSW << 32 | (A).LSW; \
                                        Val=T1*(B); \
                                        (Value).MSW = Val >> 32; \
                                        (Value).LSW = (U32)Val; \
                                        }

/*Value=Value<<Shift, where Value is U64 type*/
#define I64_ShiftLeft(Shift,Value)      { register long long T1, T2, Val; \
                                        T1 = (long long)(Value).MSW << 32 | (Value).LSW; \
                                        Val=T1 << Shift; \
                                        (Value).MSW = Val >> 32; \
                                        (Value).LSW = (U32)Val; \
                                        }

/*Value=Value>>Shift, where Value is U64 type*/
#define I64_ShiftRight(Shift,Value)     { register long long T1, T2, Val; \
                                        T1 = (long long)(Value).MSW << 32 | (Value).LSW; \
                                        Val=T1 >> Shift; \
                                        (Value).MSW = Val >> 32; \
                                        (Value).LSW = (U32)Val; \
                                        }

#define  FS_MAX_FILE_SIZE  (U64){0xFFFFFE00, 0xFF}

#define ll_geth(a)	(unsigned int)(a>>32)
#define ll_getl(a)	(unsigned int)(a&0xFFFFFFFF)

int example_S64(void)
{
	unsigned int a, b, c;
	a = 484003;
	b = 860909416;
	c = ((a-b)*1000)/1000;
#ifdef EXAMPLE_S64
	printf("c=%u\n", c);
	int int_pos[3]={0x1, 0x0FFFFFFF, 0x80000011};
	long long seek_pos;
	char str[256];

        seek_pos = (((long long) int_pos[0]) << 32) + (long long)int_pos[1];
	//seek_pos = 0xFFFEDCBA98;
	printf("seek_pos [0x%x 0x%x] \n", ll_geth(seek_pos), ll_getl(seek_pos));
	printf("22 % 7 = %d %d %d\n",  60000/1001, 60000%1001, 1000*(60000/1001)+(1000*(60000%1001))/1001);

	printf("FS_MAX_FILE_SIZE is (0x%x 0x%x)\n", FS_MAX_FILE_SIZE.MSW, FS_MAX_FILE_SIZE.LSW);
       
	memset(str, 0, sizeof(char)*256); 
        printf("size of %s is %d\n", str, strlen(str)); 
#endif
	return 0;
}
