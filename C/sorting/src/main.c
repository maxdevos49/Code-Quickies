#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void bubble_sort(int arr[], int size);
void selection_sort(int arr[], int size);
void quick_sort(int arr[], int low, int high);

int is_sorted(int arr[], int size) {
  for (int i = 0; i < size - 1; i++) {
    if (arr[i] > arr[i + 1]) {
      return i; // Not sorted
    }
  }
  return 0; // Sorted
}

void print_array(int arr[], int size) {
  for (int i = 0; i < size; i++) {
    printf("%03d ", arr[i]);
  }
  printf("\n");
}

int main(void) {
  printf("Enter the size of the array: ");

  int size;
  if (scanf("%d", &size) != 1) {
    fprintf(stderr, "Failed to read size.\n");
    return EXIT_FAILURE;
  }

  srand(time(NULL));

  int *arr = (int *)malloc(size * sizeof(int));
  if (arr == NULL) {
    perror("Could not allocate array");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < size; i++) {
    arr[i] = rand() % 1000;
  }

  // printf("Unsorted array: ");
  // print_array(arr, size);

  clock_t start = clock();
  // bubble_sort(arr, size);
  // selection_sort(arr, size);
  quick_sort(arr, 0, size - 1);
  clock_t end = clock();

  double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

  // printf("Sorted array: \n");
  // print_array(arr, size);

  int unsorted_index = is_sorted(arr, size);
  if (unsorted_index == 0) {
    printf("Array is correctly sorted!\n");
  } else {
    printf("Array is NOT sorted correctly! Index: %d\n", unsorted_index);
  }

  printf("Time take to sort: %.6f seconds\n", time_taken);
  free(arr);

  return EXIT_SUCCESS;
}

/**
 * Bubble Sort - Easiest
 * Bubble Sort is straightforward and intuitive. It repeatedly compares adjacent
 * elements and swaps them if they’re in the wrong order. The logic is simple:
 * keep passing through the list until no more swaps are needed. A basic
 * implementation requires just nested loops and a comparison, making it
 *beginner -friendly. However, its simplicity comes at the cost of efficiency
 *(O(n²) time complexity).
 */
void bubble_sort(int arr[], int size) {
  int swapped;
  do {
    swapped = 0;

    for (int i = 0; i < size - 1; i++) {
      int current = arr[i];
      int next = arr[i + 1];

      if (current > next) {

        int temp = next;
        arr[i + 1] = current;
        arr[i] = temp;

        swapped = 1;
      }
    }
  } while (swapped);
}

/**
 * Selection Sort - Moderate
 * Selection Sort is slightly trickier than Bubble Sort but still relatively
 * easy. It divides the list into a sorted and unsorted portion, repeatedly
 * finding the minimum (or maximum) element in the unsorted part and placing it
 * at the end of the sorted part. The concept is clear, but implementing it
 * involves managing the minimum index, which adds a bit more complexity than
 * Bubble Sort’s direct swaps. It also runs in O(n²) time.
 */
void selection_sort(int arr[], int size) {
  // Unsorted portion of the array section.
  int start = 0;
  int end = size - 1;

  while (start < end) {
    int min = INT_MAX;
    int minIndex;

    int max = INT_MIN;
    int maxIndex;

    // Scan the array and find min and max values.
    for (int i = start; i <= end; i++) {
      int current = arr[i];
      if (current < min) {
        min = current;
        minIndex = i;
      }

      if (current > max) {
        max = current;
        maxIndex = i;
      }
    }

    int swap;

    // Swap max to current end position.
    swap = arr[end];
    arr[end] = max;
    arr[maxIndex] = swap;

    // Swap min to current start position.
    if (minIndex != end) {
      swap = arr[start];
      arr[start] = min;
      arr[minIndex] = swap;

      start++;
    }

    end--;
  }
}

/**
 * Quick Sort - Hardest
 * Quick Sort is more challenging to implement due to its recursive nature and
 * the need to choose a pivot. It partitions the array around a pivot, then
 * recursively sorts the subarrays. While the idea is elegant, coding it
 * requires handling the partitioning logic correctly (e.g., choosing a pivot,
 * swapping elements, and ensuring the recursion terminates). Its average time
 * complexity of O(n log n) is great, but the implementation demands more care
 * than the others.
 */
void quick_sort(int arr[], int low, int high) {
  if (low < high) {
    int pivot = arr[high];
    int pi = low - 1;

    for (int j = low; j < high; j++) {
      if (arr[j] < pivot) {
        pi++;

        int temp = arr[pi];
        arr[pi] = arr[j];
        arr[j] = temp;
      }
    }

    pi++;

    int temp = arr[pi];
    arr[pi] = pivot;
    arr[high] = temp;

    quick_sort(arr, low, pi - 1);
    quick_sort(arr, pi + 1, high);
  }
}
