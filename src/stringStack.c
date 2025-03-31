#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure representation of a string stack
typedef struct {
    char **data;     // Array holding pointers to strings
    size_t capacity; // Maximum number of strings that can be stored
    size_t top;      // Index of the top element in the stack
} StringStack;

// Initialize the string stack with a given initial capacity
bool initializeStringStack(StringStack *stack, size_t initialCapacity) {
    stack->data = (char **)calloc(initialCapacity, sizeof(char *));
    if (stack->data == NULL) {
        fprintf(stderr, "Failed to allocate memory for string stack\n");
        return false;
    }

    stack->capacity = initialCapacity;
    stack->top = 0;
    return true;
}

// Free the memory allocated for the string stack
void freeStringStack(StringStack *stack) {
    if (stack == NULL) {
        return;
    }
    for (size_t i = 0; i < stack->top; i++) {
        // Free each string memory
        free(stack->data[i]);
    }

    free(stack->data);
    stack->data = NULL;
    stack->capacity = 0;
    stack->top = 0;
}

// Check if the string stack is empty
bool isEmptyStringStack(const StringStack *stack) { return stack->top == 0; }

// Check if the string stack is full
bool isFullStringStack(const StringStack *stack) {
    return stack->top == stack->capacity;
}

// Push a string onto the stack
// Double the capacity if the stack is full
bool pushStringStack(StringStack *stack, const char *value) {
    // If the stack is full, double the capacity
    if (isFullStringStack(stack)) {
        size_t newCapacity = stack->capacity * 2;
        char **newData =
            (char **)realloc(stack->data, newCapacity * sizeof(char *));
        if (newData == NULL) {
            fprintf(stderr, "Failed to reallocate memory for string stack\n");
            return false;
        }

        stack->data = newData;
        stack->capacity = newCapacity;
    }

    // Duplicate the string and store it in the stack
    stack->data[stack->top] = strdup(value);
    if (stack->data[stack->top] == NULL) {
        fprintf(stderr, "Failed to duplicate string for string stack\n");
        return false;
    }

    stack->top++;
    return true;
}

// Pop a string from the stack
// (Caller is responsible for freeing the memory)
bool popStringStack(StringStack *stack, char **value) {
    if (isEmptyStringStack(stack)) {
        fprintf(stderr, "Cannot pop from an empty string stack\n");
        return false;
    }

    stack->top--;
    *value = stack->data[stack->top];
    return true;
}

// Print the contents of the string stack
// (Caller is responsible for freeing the memory)
bool peekStringStack(const StringStack *stack, char **value) {
    if (isEmptyStringStack(stack)) {
        fprintf(stderr, "Cannot peek into an empty string stack\n");
        return false;
    }

    // We don't modify the stack, so we don't decrement the top
    *value = stack->data[stack->top - 1];

    return true;
}
// Main function to demonstrate usage of the string stack
int main(void) {
    StringStack stack;
    if (!initializeStringStack(&stack, 4)) {
        return EXIT_FAILURE;
    }

    // Push some strings onto the stack
    pushStringStack(&stack, "hello");
    pushStringStack(&stack, "world");
    pushStringStack(&stack, "cyberpunk");
    pushStringStack(&stack, "Klojure");

    // Peek at the top element
    char *topStr = NULL;
    if (peekStringStack(&stack, &topStr)) {
        printf("Top element: %s\n", topStr);
    }

    // Pop and print all elements
    printf("Popping stack:\n");
    while (!isEmptyStringStack(&stack)) {
        char *value;
        if (popStringStack(&stack, &value)) {
            printf("%s\n", value);
            free(value); // Caller frees the popped string
        }
    }

    // Clean up the stack itself
    freeStringStack(&stack);
    return EXIT_SUCCESS;
}
