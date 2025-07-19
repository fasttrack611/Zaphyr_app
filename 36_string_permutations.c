// C Program to print all permutations of a string in sorted
// order.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function two compare two characters a and b
int compare(const void* a, const void* b)
{
    return (*(char*)a - *(char*)b);
}

// function two swap two characters a and b
void swap(char* a, char* b)
{
    char t = *a;
    *a = *b;
    *b = t;
}

// function finds the index of the smallest character
int findCeil(char str[], char first, int l, int h)
{
    int ceilIndex = l;

    for (int i = l + 1; i <= h; i++)
        if (str[i] > first && str[i] < str[ceilIndex])
            ceilIndex = i;

    return ceilIndex;
}

// Print all permutations of str in sorted order
void sortedPermutations(char str[])
{
    int size = strlen(str);

    qsort(str, size, sizeof(str[0]), compare);

    int isFinished = 0;
    while (!isFinished) {
        printf("%s \n", str);

        int i;
        for (i = size - 2; i >= 0; --i)
            if (str[i] < str[i + 1])
                break;

        if (i == -1)
            isFinished = 1;
        else {

            int ceilIndex
                = findCeil(str, str[i], i + 1, size - 1);
            swap(&str[i], &str[ceilIndex]);
            qsort(str + i + 1, size - i - 1, sizeof(str[0]),
                  compare);
        }
    }
}

int main()
{
    char str[] = "123";
    sortedPermutations(str);
    return 0;
}
