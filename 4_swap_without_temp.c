// C Program to
// Swap two numbers
// No Extra Space
#include <stdio.h>

int main()
{

    int x = 10;
    int y = 20;

    printf("x: %d , y: %d\n", x, y);

    // x hold 30
    x = x + y;

    // y hold 10
    y = x - y;

    // Now, x hold 20
    x = x - y;

    printf("x: %d , y: %d\n", x, y);

    return 0;
}
