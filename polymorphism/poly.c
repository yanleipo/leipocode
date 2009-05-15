/* This program shows how to create a form of function polymorphism
 * using the C language
 *
 * The technique shown is nowhere near as powerful as what C++ or other OO
 * languages provide, but is useful for some applications.
 *
 * Compilation:
 *      First, save the program as poly.c; then
 *      compile with: gcc -o poly poly.c
 *
 * Usage:
 *      The program takes three arguments:
 *              poly TYPE X Y
 *
 *      TYPE: if equal to 1, then X and Y are taken as floating point numbers,
 *              otherwise, X and Y are taken as integers
 *
 *      X, Y : two numbers of the type specified by TYPE
 *
 *      The program will output the addition of X and Y.
 *
 * Examples:
 *
 *      $ ./poly 1 2.0 3.0
 *      Result = 5.00000
 *
 *      $ ./poly 2 2 3
 *      Result = 5
 *
 * */

#include <stdlib.h>

/* all quantities used by the program will be of type "number"
 * (a union is used to save memory) */
typedef union
{
        double floatpt;
        int    integer;
} number;

void help(void)
{
        printf(" * Usage:\n");
        printf(" *     The program takes three arguments:\n");
        printf(" *          output.exe TYPE x y:\n");
        printf(" * \n");
        printf(" * TYPE: if eeuqal to 1, then x and y are taken as floating point numbers,:\n");
        printf(" *       otherwise, x and y are taken as integers:\n");
        printf(" * \n");
        printf(" * x, y: two numbers of the type specified as TYPE:\n");
        printf(" * \n");
        printf(" * The program will output the addition of x and y.:\n");
        printf(" * \n");
}

/* the next two functions are used to add numbers. Each function knows how to
 * add a different type */
number add_floatpt(number x, number y)
{
        number result;

        result.floatpt = x.floatpt + y.floatpt;
        return result;
}

number add_integer(number x, number y)
{
        number result;

        result.integer = x.integer + y.integer;
        return result;
}

/* the next two functions are used to print the result. Each function knows how
 * to print a different type */
void print_floatpt( number x )
{
        printf( "Result = %1.5f\n", x.floatpt );
}

void print_integer( number x )
{
        printf( "Result = %d\n", x.integer );
}

int main( int argc, char *argv[] )
{
        int sel;
        number x, y, result;

        if( argc < 4 )
        {
                help();
                return 0;
        }

        /* pointers to functions.
         * functions "add" and "print" will show polymorphism */
        number (* add)(number, number) = NULL;
        void (* print)(number) = NULL;

        sel = atoi( argv[1] );

        /* make "add" and "print" point to the function of the correct type.
         * note that the option to use integers or floating-point values
         * is set at run time; there is no need to recompile. */
        if( sel == 1 ) {
                add = &add_floatpt;
                print = &print_floatpt;
                x.floatpt = atof( argv[2] );
                y.floatpt = atof( argv[3] );

        }
        else {
                add = &add_integer;
                print = &print_integer;
                x.integer = atoi( argv[2] );
                y.integer = atoi( argv[3] );
        }

        /* from this point forward, "add" and "print" can be used as
         * polymorphic functions. As long as the program does all its
         * calculations using the "number" type, it doesn't care about
         * whether the underlying types are integers or floating point
         * numbers */
        result = add( x, y );
        print( result );

        return 0;
}

