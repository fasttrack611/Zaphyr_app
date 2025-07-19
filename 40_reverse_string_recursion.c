// C program to reverse
// String using recursion
#include <stdio.h>
#include <string.h>

// Using Iteration for reverse
void reverse_iteration(char* str)
{
    int i = 0;
    int j = strlen(str) - 1;

    for (; i < j; i++, j--) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

// Using recursion for reverse
void reverse(char* str)
{
    if (*str) {
        reverse(str + 1);
        printf("%c", *str);
    }
}

int main()
{
    char a[] = "Geeks for Geeks";
    printf("Orignal string:%s\n", a);

    reverse_iteration(a);
    printf("Reverse the string(iteration):%s\n", a);

    printf("Using recursion for reverse:");
    reverse(a);

    return 0;
}
