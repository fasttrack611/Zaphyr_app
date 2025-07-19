// C Program for
// sorting array using
// Quick sort
#include <stdio.h>

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int array[], int low, int high)
{
    int pivot = array[high];

    int i = (low - 1);

    // compare elements with the pivot
    for (int j = low; j < high; j++) {
        if (array[j] <= pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }

    // swap the pivot element with the greater element at i
    swap(&array[i + 1], &array[high]);

    return (i + 1);
}

void quickSort(int array[], int low, int high)
{
    if (low < high) {
        int pi = partition(array, low, high);
        quickSort(array, low, pi - 1);
        quickSort(array, pi + 1, high);
    }
}

void printArray(int array[], int n)
{
    for (int i = 0; i < n; ++i) {
        printf("%d  ", array[i]);
    }
    printf("\n");
}

int main()
{
    int arr[] = { 28, 7, 20, 1, 10, 3 , 6 };

    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Unsorted Array:");
    printArray(arr, n);

    quickSort(arr, 0, n - 1);

    printf("Sorted array :");
    printArray(arr, n);

    return 0;
}
