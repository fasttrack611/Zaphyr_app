// C Program to implement
// Sorting Algorithms
#include <stdio.h>

// A function to implement bubble sort
void bubble_sort(int* arr, int n)
{
    for (int j = 0; j < n - 1; j++) {

        // Last j elements are already in place
        for (int i = 0; i < n - j - 1; i++) {
            if (arr[i] > arr[i + 1]) {
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
            }
        }
    }
}

// A function to implement swaping
void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// A function to implement selectionSort
void selectionSort(int arr[], int n)
{

    // One by one move boundary of unsorted subarray
    for (int i = 0; i < n - 1; i++) {
        // Find the minimum element in unsorted array
        int min_idx = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;

        // Swap the found minimum element
        // with the first element
        if (min_idx != i)
            swap(&arr[min_idx], &arr[i]);
    }
}

void insertionSort(int arr[], int n)
{

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        // Move elements of arr that are
        // greater than key, to one position ahead
        // of their current position
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

int main()
{
    int arr1[] = { 9, 4, 3, 11, 1, 5 };
    int arr2[] = { 4, 3, 9, 1, 5, 11 };
    int arr3[] = { 5, 1, 11, 3, 4, 9 };
    int n = 6;

    printf("Non-Sorted array: ");
    for (int i = 0; i < n; i++)
        printf("%d ", arr1[i]);
    printf("\n");

    // sort array
    bubble_sort(arr1, n);

    // printing array
    printf("Sorted array using Bubble sort: ");
    for (int i = 0; i < n; i++)
        printf("%d ", arr1[i]);
    printf("\n");

    printf("Non-Sorted array: ");
    for (int i = 0; i < n; i++)
        printf("%d ", arr2[i]);
    printf("\n");

    // sort array
    insertionSort(arr2, n);

    // printing array
    printf("Sorted array using Insertion Sort: ");
    for (int i = 0; i < n; i++)
        printf("%d ", arr2[i]);
    printf("\n");

    printf("Non-Sorted array: ");
    for (int i = 0; i < n; i++)
        printf("%d ", arr3[i]);
    printf("\n");

    // sort array
    selectionSort(arr3, n);

    // printing array
    printf("Sorted array using Selection Sort: ");
    for (int i = 0; i < n; i++)
        printf("%d ", arr3[i]);
    printf("\n");

    return 0;
}
