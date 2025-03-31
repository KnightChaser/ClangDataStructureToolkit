#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Structure to hold the dynamic integer array
typedef struct {
    int *data;       // Pointer to the array holding the data
    size_t size;     // Current number of elements in the array
    size_t capacity; // Current capacity of the array (adjustable)
} IntegerDynamicArray;

// Initialize the dynamic integer array with the given initial capacity
bool initializeIntegerDynamicArray(IntegerDynamicArray *array,
                                   size_t initialCapacity) {
    array->data = (int *)calloc(initialCapacity, sizeof(int));
    if (array->data == NULL) {
        fprintf(stderr, "Memory allocation failed during initialization\n");
        return false;
    }
    array->size = 0;
    array->capacity = initialCapacity;
    return true;
}

// Free memory allocated for the dynamic integer array
void freeIntegerDynamicArray(IntegerDynamicArray *array) {
    free(array->data);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
}

// Append a value to the end of the dynamic integer array
bool appendIntegerDynamicArray(IntegerDynamicArray *array, int value) {
    if (array->size == array->capacity) {
        // If the array is full, double the capacity to hold more elements
        size_t newCapacity = array->capacity * 2;
        int *newData = (int *)realloc(array->data, newCapacity * sizeof(int));
        if (newData == NULL) {
            fprintf(stderr, "Memory allocation failed during append\n");
            return false;
        }
        array->data = newData;
        array->capacity = newCapacity;
    }
    array->data[array->size++] = value;
    return true;
}

// Find the index of the first occurrence of the given value in the dynamic
// integer array Returns true and sets *index if found; returns false otherwise.
bool findIntegerDynamicArray(IntegerDynamicArray *array, int value,
                             size_t *index) {
    for (size_t i = 0; i < array->size; i++) {
        if (array->data[i] == value) {
            *index = i;
            return true;
        }
    }
    return false;
}

// Delete the first occurrence of the given value in the dynamic integer array
bool deleteIntegerDynamicArray(IntegerDynamicArray *array, int value) {
    size_t index;
    if (!findIntegerDynamicArray(array, value, &index)) {
        // Value not found
        fprintf(stderr, "Value %d not found in the array, cannot delete\n",
                value);
        return false;
    }
    // Shift elements to fill the gap
    for (size_t i = index; i < array->size - 1; i++) {
        array->data[i] = array->data[i + 1];
    }
    array->size--;
    return true;
}

// Main function to demonstrate the usage of the dynamic integer array
int main(void) {
    IntegerDynamicArray vec;
    if (!initializeIntegerDynamicArray(&vec, 4)) {
        return EXIT_FAILURE;
    }

    // Append values 1 through 10
    for (int i = 1; i <= 10; i++) {
        if (!appendIntegerDynamicArray(&vec, i)) {
            freeIntegerDynamicArray(&vec);
            return EXIT_FAILURE;
        }
    }

    // Print the current elements
    printf("Array elements after appending 1 through 10:\n");
    for (size_t i = 0; i < vec.size; i++) {
        printf("%d ", vec.data[i]);
    }
    printf("\n");

    // Find a value in the array
    size_t index;
    int searchValue = 5;
    if (findIntegerDynamicArray(&vec, searchValue, &index)) {
        printf("Found %d at index %zu\n", searchValue, index);
    } else {
        printf("%d not found in the array\n", searchValue);
    }

    // Delete a value from the array
    int deleteValue = 3;
    if (deleteIntegerDynamicArray(&vec, deleteValue)) {
        printf("Deleted %d from the array\n", deleteValue);
    } else {
        printf("Failed to delete %d from the array\n", deleteValue);
    }

    // Print the array after deletion
    printf("Array elements after deletion:\n");
    for (size_t i = 0; i < vec.size; i++) {
        printf("%d ", vec.data[i]);
    }
    printf("\n");

    // Clean up
    freeIntegerDynamicArray(&vec);
    return EXIT_SUCCESS;
}
