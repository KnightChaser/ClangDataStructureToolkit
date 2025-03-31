#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Structure representing an integer stack
typedef struct {
    int *data;   // Array to store the elements
    size_t size; // Size of the array
    size_t top;  // Index of the top element
} integerStack;

// Initialize the integer stack with the given initial capacity
bool initializeIntegerStack(integerStack *stack, size_t initialCapacity) {
    stack->data = (int *)calloc(initialCapacity, sizeof(int));
    if (stack->data == NULL) {
        fprintf(stderr, "Failed to allocate memory for the stack.\n");
        return false;
    }

    stack->size = initialCapacity;
    stack->top = 0;
    return true;
}

// Free the memory allocated for the integer stack
void freeIntegerStack(integerStack *stack) {
    free(stack->data);
    stack->data = NULL;
    stack->size = 0;
    stack->top = 0;
}

// Check if the integer stack is empty
bool isEmptyIntegerStack(integerStack *stack) { return stack->top == 0; }

// Check if the integer stack is full
bool isFullIntegerStack(integerStack *stack) {
    return stack->top == stack->size;
}

// Push an element onto the integer stack
// When the stack is full, the function will realloc() the array
// to double the size of the original array
bool pushIntegerStack(integerStack *stack, int value) {
    if (isFullIntegerStack(stack)) {
        size_t newSize = stack->size * 2;
        int *newData = (int *)realloc(stack->data, newSize * sizeof(int));
        if (newData == NULL) {
            fprintf(stderr, "Failed to reallocate memory for the stack.\n");
            return false;
        }

        stack->data = newData;
        stack->size = newSize;
    }

    stack->data[stack->top] = value;
    stack->top++;

    return true;
}

// Pop an element from the integer stack
bool popIntegerStack(integerStack *stack, int *value) {
    if (isEmptyIntegerStack(stack)) {
        fprintf(stderr, "The stack is empty.\n");
        return false;
    }

    stack->top--;
    *value = stack->data[stack->top];

    return true;
}

// Peek the top element of the integer stack
bool peekIntegerStack(integerStack *stack, int *value) {
    if (isEmptyIntegerStack(stack)) {
        fprintf(stderr, "The stack is empty.\n");
        return false;
    }

    // Peek does not modify the stack
    *value = stack->data[stack->top - 1];

    return true;
}

// Main function to demonstrate stack usage
int main(void) {
    integerStack stack;
    if (!initializeIntegerStack(&stack, 4)) {
        return EXIT_FAILURE;
    }

    // Push values 1 through 10 onto the stack
    for (int i = 1; i <= 10; i++) {
        if (!pushIntegerStack(&stack, i)) {
            freeIntegerStack(&stack);
            return EXIT_FAILURE;
        }
    }

    // Peek at the top element
    int topValue;
    if (peekIntegerStack(&stack, &topValue)) {
        printf("Top element: %d\n", topValue);
    }

    // Pop and print all elements
    printf("Popping stack:\n");
    while (!isEmptyIntegerStack(&stack)) {
        int value;
        if (popIntegerStack(&stack, &value)) {
            printf("%d ", value);
        }
    }
    printf("\n");

    // Clean up
    freeIntegerStack(&stack);
    return EXIT_SUCCESS;
}
