// C program  to calculate the Power of a Number using
// Recursion
#include <stdio.h>

int power(int a, int b)
{
    if (b == 0)
        return 1;

    return power(a, b - 1) * a;
}

int main()
{
    int a = 4, b = 5;

    int ans = power(a, b);

    printf("%d", ans);
    return 0;
}
