#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Int64LinkedListNode {
    int64_t value;
    struct Int64LinkedListNode *prev;
    struct Int64LinkedListNode *next;
} Int64LinkedListNode;

typedef struct {
    Int64LinkedListNode *head;
    Int64LinkedListNode *tail;
    size_t size;
} Int64LinkedList;

// Create a new Int64LinkedList (node object of the list)
static inline Int64LinkedListNode *int64LinkedListNodeCreate(int64_t value) {
    Int64LinkedListNode *node = calloc(1, sizeof(Int64LinkedListNode));
    if (!node) {
        fprintf(stderr, "Failed to create Int64ListNode\n");
        return NULL;
    }

    node->value = value;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

// Create an empty int64_t linked list
Int64LinkedList *int64LinkedListCreate() {
    Int64LinkedList *list = calloc(1, sizeof(Int64LinkedList));
    if (!list) {
        fprintf(stderr, "Failed to create Int64LinkedList\n");
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    return list;
}

// Destroy the list and free all nodes.
void int64LinkedListDestroy(Int64LinkedList *list) {
    if (!list) {
        return;
    }

    Int64LinkedListNode *current = list->head;
    while (current != NULL) {
        Int64LinkedListNode *next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

// Insert a value at the front;
// Returns the new node on success or NULL on failure.
Int64LinkedListNode *int64LinkedListInsertFront(Int64LinkedList *list,
                                                int64_t value) {
    Int64LinkedListNode *node = int64LinkedListNodeCreate(value);
    if (!node) {
        return NULL;
    }

    node->next = list->head;
    if (list->head) {
        // If the given linked list already had >= 1 node(s)
        list->head->prev = node;
    } else {
        // If this node is the first node of the given linked list
        list->tail = node;
    }

    list->head = node;
    list->size++;

    return node;
}

// Insert a value at the back;
// Returns the new node on success or NULL on failure.
Int64LinkedListNode *int64LinkedListInsertBack(Int64LinkedList *list,
                                               int64_t value) {
    Int64LinkedListNode *node = int64LinkedListNodeCreate(value);
    if (!node) {
        return NULL;
    }

    node->prev = list->tail;
    if (list->tail) {
        // If the given linked list already had >= 1 node(s)
        list->tail->next = node;
    } else {
        // If this node is the first node of the given linked list
        list->head = node;
    }

    list->tail = node;
    list->size++;

    return node;
}

// Delete the first node of the list;
// Returns true if the node was deleted successfully, false otherwise.
bool int64LinkedListRemoveNode(Int64LinkedList *list,
                               Int64LinkedListNode *node) {
    if (!list || !node) {
        return false;
    }

    if (node->prev) {
        // If the node is not 1st node of the given linked list
        node->prev->next = node->next;
    } else {
        // If the node is the 1st node of the given linked list
        list->head = node->next;
    }

    if (node->next) {
        // If the node is not the last node of the given linked list
        node->next->prev = node->prev;
    } else {
        // If the node is the last node of the given linked list
        list->tail = node->prev;
    }

    free(node);
    list->size--;

    return true;
}

// Find the first node with the given value;
// Returns the node on success or NULL if not found.
Int64LinkedListNode *int64LinkedListFind(const Int64LinkedList *list,
                                         int64_t value) {
    // Linear search
    for (Int64LinkedListNode *current = list->head; current != NULL;
         current = current->next) {
        if (current->value == value) {
            return current;
        }
    }
    return NULL;
}

// Remove the first occurrence of a value;
// Returns true if the value was found and removed, false otherwise.
bool int64LinkedListRemoveValue(Int64LinkedList *list, int64_t value) {
    Int64LinkedListNode *node = int64LinkedListFind(list, value);
    if (!node) {
        return false;
    }
    return int64LinkedListRemoveNode(list, node);
}

// Check if the list is empty;
static inline bool int64LinkedListIsEmpty(const Int64LinkedList *list) {
    return list->size == 0;
}

int main(void) {
    Int64LinkedList *list = int64LinkedListCreate();
    if (!list) {
        return EXIT_FAILURE;
    }

    // Push values 1...5 at back
    for (int64_t i = 1; i <= 5; i++) {
        if (!int64LinkedListInsertBack(list, i)) {
            int64LinkedListDestroy(list);
            return EXIT_FAILURE;
        }
    }

    // Push value 0 at front
    int64LinkedListInsertFront(list, 0);

    // Print forward traversal
    printf("Forward traversal: ");
    for (Int64LinkedListNode *current = list->head; current != NULL;
         current = current->next) {
        printf("%ld ", current->value);
    }

    // Print backward traversal
    printf("\nBackward traversal: ");
    for (Int64LinkedListNode *current = list->tail; current != NULL;
         current = current->prev) {
        printf("%ld ", current->value);
    }

    // Remove value 3
    if (int64LinkedListRemoveValue(list, 3)) {
        printf("\nRemoved value 3\n");
    } else {
        printf("\nValue 3 not found\n");
    }

    // Print forward traversal after removal
    printf("Forward traversal after removal: ");
    for (Int64LinkedListNode *current = list->head; current != NULL;
         current = current->next) {
        printf("%ld ", current->value);
    }
    printf("\n");

    int64LinkedListDestroy(list);

    return EXIT_SUCCESS;
}
