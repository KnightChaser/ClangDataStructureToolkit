#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node struture for generically-typed linked list
typedef struct Node {
    void *data;
    struct Node *next;
} LinkedListNode;

// Generic linked list structure
typedef struct {
    LinkedListNode *head;
    LinkedListNode *tail;
    // Number of nodes in the list
    size_t size;
    // Function pointer to free node data (can be NULL if not needed)
    void (*freeFunction)(void *);
    // Function pointer to compare node data (can be NULL if not needed)
    int (*compareFunction)(void *, void *);
} LinkedList;

// Initialize linked list
void initLinkedList(LinkedList *list, void (*freeFunction)(void *),
                    int (*compareFunction)(void *, void *)) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->freeFunction = freeFunction;
    list->compareFunction = compareFunction;
}

// Create a new node with given data
LinkedListNode *createNode(void *data) {
    LinkedListNode *node = (LinkedListNode *)calloc(1, sizeof(LinkedListNode));
    if (node == NULL) {
        fprintf(stderr, "Failed to allocate memory for new node\n");
        return NULL;
    }

    node->data = data;
    node->next = NULL;
    return node;
}

// Add a new node to the end of the list
bool appendLinkedList(LinkedList *list, void *data) {
    LinkedListNode *node = createNode(data);
    if (node == NULL) {
        return false;
    }

    if (list->head == NULL) {
        // If list is empty, set head and tail to new node
        list->head = node;
        list->tail = node;
    } else {
        // If list is not empty, append new node to the end
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;
    return true;
}

// Find a node with data equal to target using compareFunction
// Returns the first node found with matching data, or NULL if not found
LinkedListNode *findLinkedList(LinkedList *list, void *target) {
    LinkedListNode *current = list->head;
    while (current) {
        if (list->compareFunction != NULL) {
            // If compareFunction is provided, use it to compare data
            if (list->compareFunction(current->data, target) == 0) {
                return current;
            }
        } else {
            // Use default comparison if compareFunction is not provided
            if (current->data == target) {
                return current;
            }
        }
        // Move to next node
        current = current->next;
    }
    return NULL;
}

// Remove a node with data equal to target using compareFunction.
// Returns true if a node is removed, false if not found.
bool removeLinkedList(LinkedList *list, void *target) {
    LinkedListNode *current = list->head;
    LinkedListNode *previous = NULL;

    while (current) {
        if (list->compareFunction != NULL) {
            // If compareFunction is provided, use it to compare data
            if (list->compareFunction(current->data, target) == 0) {
                break;
            }
        } else {
            // If compareFunction is not provided, use default comparison
            if (current->data == target) {
                break;
            }
        }

        previous = current;
        current = current->next;
    }

    // If target is not found, return false
    if (current == NULL) {
        return false;
    }

    // If target is found, remove the node
    // unlink the node from the list
    if (previous == NULL) {
        // If the node to remove is the head
        list->head = current->next;
    } else {
        previous->next = current->next;
    }

    // If the node to remove is the tail
    if (current == list->tail) {
        list->tail = previous;
    }

    // If freeFunction is provided, use it to free data
    if (list->freeFunction != NULL) {
        list->freeFunction(current->data);
    }

    free(current);
    list->size--;

    return true;
}

// Free all nodes in the list
void freeLinkedList(LinkedList *list) {
    LinkedListNode *current = list->head;
    while (current) {
        LinkedListNode *temp = current;
        current = current->next;
        if (list->freeFunction != NULL) {
            list->freeFunction(temp->data);
        }

        free(temp);
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

// ------- Example usage with integer data -------

// Custom free function for integer data
void freeInt(void *data) { free((int *)data); }

// Custom compare function for integer data
int compareInteger(void *a, void *b) { return *(int *)a - *(int *)b; }

int main(void) {
    LinkedList list;
    // Initialize list with free and compare functions for integers
    initLinkedList(&list, freeInt, compareInteger);

    // Append some integers to the list
    for (int i = 0; i < 5; i++) {
        int *num = malloc(sizeof(int));
        if (!num) {
            fprintf(stderr, "Memory allocation failed for integer\n");
            freeLinkedList(&list);
            return EXIT_FAILURE;
        }
        *num = i;
        appendLinkedList(&list, num);
    }

    // Find an element (e.g., integer 3)
    int key = 3;
    LinkedListNode *found = findLinkedList(&list, &key);
    if (found)
        printf("Found: %d\n", *(int *)found->data);
    else
        printf("Not found: %d\n", key);

    // Remove an element (e.g., integer 2)
    key = 2;
    if (removeLinkedList(&list, &key))
        printf("Removed element: %d\n", key);
    else
        printf("Element %d not found for removal\n", key);

    // Print remaining elements
    printf("Remaining elements:\n");
    LinkedListNode *current = list.head;
    while (current) {
        printf("%d ", *(int *)current->data);
        current = current->next;
    }
    printf("\n");

    // Clean up the entire list
    freeLinkedList(&list);
    return EXIT_SUCCESS;
}
