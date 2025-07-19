// C Program to reverse
// An array
#include <stdio.h>

void reverse(int* arr, int n)
{
    // Swapping front and back elements.
    for (int i = 0, j = n - 1; i < j; i++, j--) {
        int ele = arr[i];
        arr[i] = arr[j];
        arr[j] = ele;
    }
}

int main()
{

    int arr[] = { 1, 2, 3, 4, 5 };
    // Function Call
    reverse(arr, 5);

    // reverse array element printing
    for (int i = 0; i < 5; i++)
        printf("%d ", arr[i]);

    return 0;
}
