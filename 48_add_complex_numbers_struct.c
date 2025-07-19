// C program to demonstrate
// addition of complex numbers
#include <stdio.h>

// define a structure for complex number
typedef struct complexNumber {
    int real;
    int img;
} complex;


complex add(complex x, complex y)
{
    // define a new complex number.
    complex add;

    // add similar type together
    add.real = x.real + y.real;
    add.img = x.img + y.img;

    return (add);
}

int main()
{

    // define three complex type numbers
    complex x, y, sum;

    // first complex number
    x.real = 4;
    x.img = 5;

    // second complex number
    y.real = 7;
    y.img = 11;

    // printing both complex numbers
    printf(" x = %d + %di\n", x.real, x.img);
    printf(" y = %d + %di\n", y.real, y.img);

    // call add(a,b) function and
    // pass complex numbers a & b
    // as an parameter.
    sum = add(x, y);

    // print result
    printf("\n sum = %d + %di", sum.real, sum.img);

    return 0;
}
