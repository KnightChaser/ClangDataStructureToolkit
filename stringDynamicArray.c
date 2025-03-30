#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node structure for each string element
typedef struct stringDynamicArrayNode {
    char *data;                          // The string data
    struct stringDynamicArrayNode *next; // Pointer to the next node
} stringDynamicArrayNode;

// Structure for the dynamic string array
typedef struct {
    stringDynamicArrayNode *head; // Pointer to the first node
    stringDynamicArrayNode *tail; // Pointer to the last node
    size_t size;                  // Number of elements in the array
} stringDynamicArray;

// Initialize the dynamic string array
bool initializeStringDynamicArray(stringDynamicArray *array) {
    if (array == NULL) {
        return false;
    }

    array->head = NULL;
    array->tail = NULL;
    array->size = 0;
    return true;
}

// Free all memory used by the dynamic string array
void freeStringDynamicArray(stringDynamicArray *array) {
    if (array == NULL) {
        return;
    }

    stringDynamicArrayNode *current = array->head;
    while (current != NULL) {
        stringDynamicArrayNode *next = current->next;
        free(current->data); // Free the string data (char *)
        free(current);       // Free the node itself
        current = next;
    }

    array->head = NULL;
    array->tail = NULL;
    array->size = 0;
}

// Append a string to the dynamic string array
bool appendStringDynamicArray(stringDynamicArray *array, const char *string) {
    if (array == NULL || string == NULL) {
        return false;
    }

    // Allocate a new node
    stringDynamicArrayNode *newNode =
        (stringDynamicArrayNode *)calloc(1, sizeof(stringDynamicArrayNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed for the new node\n");
        return false;
    }

    // Dupli cate the string
    newNode->data = strdup(string);
    if (newNode->data == NULL) {
        fprintf(stderr, "Memory allocation failed for the string\n");
        free(newNode);
        return false;
    }
    newNode->next = NULL;

    if (array->head == NULL) {
        // The array is empty
        array->head = newNode;
        array->tail = newNode;
    } else {
        // Append the new node to the end of the array
        array->tail->next = newNode;
        array->tail = newNode;
    }

    array->size++;
    return true;
}

// Find the index of the first occurrence of a string in the dynamic string
// array If found, returns true and sets *index; otherwise, returns false.
bool findStringDynamicArray(stringDynamicArray *array, const char *string,
                            size_t *index) {
    if (array == NULL || string == NULL || index == NULL) {
        fprintf(stderr, "Invalid arguments\n");
        return false;
    }

    size_t idx = 0;
    size_t stringLength = strlen(string);
    for (stringDynamicArrayNode *current = array->head; current != NULL;
         current = current->next) {
        if (strncmp(string, current->data, stringLength) == 0) {
            *index = idx;
            return true;
        }
        idx++;
    }

    // The string is not found
    return false;
}

// Delete the string at the specified index in the dynamic string array
// Returns true if the string is deleted; otherwise, returns false.
bool deleteStringDynamicArray(stringDynamicArray *array, const char *string) {
    if (array == NULL || string == NULL) {
        fprintf(stderr, "Invalid arguments\n");
        return false;
    }

    stringDynamicArrayNode *previous = NULL;
    stringDynamicArrayNode *current = array->head;

    size_t stringLength = strlen(string);
    while (current) {
        if (strncmp(current->data, string, stringLength) == 0) {
            // If the string is foundr, delete it
            if (previous) {
                // If the target node is not the first node
                previous->next = current->next;
            } else {
                // If the target node is the first node
                array->head = current->next;
            }

            // Update the tail if the target node is the last node
            if (current == array->tail) {
                array->tail = previous;
            }

            // Deallocate the memory used by the string and the node
            free(current->data);
            free(current);
            array->size--;
            return true;
        }

        // Move to the next node
        previous = current;
        current = current->next;
    }

    // fprintf(stderr, "The string \"%s\" is not found\n", string);
    return false;
}

// Main function to demonstrate usage
int main(void) {
    stringDynamicArray sda;

    // Initialize the string dynamic array
    if (!initializeStringDynamicArray(&sda)) {
        return EXIT_FAILURE;
    }

    // Append some strings
    appendStringDynamicArray(&sda, "apple");
    appendStringDynamicArray(&sda, "banana");
    appendStringDynamicArray(&sda, "cherry");
    appendStringDynamicArray(&sda, "date");

    // Print all strings in the array
    printf("String Dynamic Array elements:\n");
    for (stringDynamicArrayNode *node = sda.head; node; node = node->next) {
        printf("%s\n", node->data);
    }

    // Find a string
    size_t idx;
    const char *searchString = "banana";
    if (findStringDynamicArray(&sda, searchString, &idx)) {
        printf("Found \"%s\" at index %zu\n", searchString, idx);
    } else {
        printf("\"%s\" not found in the array\n", searchString);
    }

    // Delete a string
    if (deleteStringDynamicArray(&sda, "cherry")) {
        printf("Deleted \"cherry\" from the array\n");
    } else {
        printf("Failed to delete \"cherry\"\n");
    }

    // Print the array after deletion
    printf("Array after deletion:\n");
    for (stringDynamicArrayNode *node = sda.head; node; node = node->next) {
        printf("%s\n", node->data);
    }

    // Free all allocated memory
    freeStringDynamicArray(&sda);

    return EXIT_SUCCESS;
}
