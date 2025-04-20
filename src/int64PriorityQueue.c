#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int64_t *data;   // heap array
    size_t size;     // number of elements in the heap
    size_t capacity; // maximum number of elements in the heap
} Int64PriorityQueue;

// Index helpers
static inline size_t getParentIndex(size_t i) { return (i - 1) / 2; }
static inline size_t getLeftChildIndex(size_t i) { return 2 * i + 1; }
static inline size_t getRightChildIndex(size_t i) { return 2 * i + 2; }

// Swap two elements in the heap
static inline void int64Swap(int64_t *a, int64_t *b) {
    int64_t tmp = *a;
    *a = *b;
    *b = tmp;
}

// Resize the underlying array
static bool int64PriorityQueueResize(Int64PriorityQueue *pq,
                                     size_t newCapacity) {
    int64_t *newData = realloc(pq->data, newCapacity * sizeof(int64_t));
    if (!newData) {
        fprintf(stderr, "Memory (re)allocation failed for capacity %zu\n",
                newCapacity);
        return false;
    }

    pq->data = newData;
    pq->capacity = newCapacity;
    return true;
}

// Create a new int64_t priority queue
Int64PriorityQueue *int64PriorityQueueCreate(size_t initialCapacity) {
    if (initialCapacity == 0) {
        fprintf(stderr, "Initial capacity must be > 0 (got %zu)\n",
                initialCapacity);
        return NULL;
    }

    Int64PriorityQueue *pq = calloc(1, sizeof(*pq));
    if (!pq) {
        fprintf(stderr, "Memory allocation failed for Int64PriorityQueue\n");
        return NULL;
    }

    pq->data = calloc(initialCapacity, sizeof(*pq->data));
    if (!pq->data) {
        fprintf(stderr, "Memory allocation failed for data array\n");
        free(pq);
        return NULL;
    }

    pq->size = 0;
    pq->capacity = initialCapacity;
    return pq;
}

// Destroy the int64_t priority queue memory
void int64PriorityQueueDestroy(Int64PriorityQueue *pq) {
    if (!pq)
        return;
    free(pq->data);
    free(pq);
}

// Restore the max‐heap property moving node i up
static void heapifyUp(Int64PriorityQueue *pq, size_t i) {
    while (i > 0) {
        size_t parent = getParentIndex(i);
        if (pq->data[parent] >= pq->data[i])
            break;
        int64Swap(&pq->data[parent], &pq->data[i]);
        i = parent;
    }
}

// Restore the max‐heap property moving node i down
static void heapifyDown(Int64PriorityQueue *pq, size_t i) {
    while (true) {
        size_t left = getLeftChildIndex(i);
        size_t right = getRightChildIndex(i);
        size_t largest = i;

        // Check if left child exists and is greater than current largest
        if (left < pq->size && pq->data[left] > pq->data[largest])
            largest = left;
        if (right < pq->size && pq->data[right] > pq->data[largest])
            largest = right;
        if (largest == i) {
            // The max-heap property is satisfied
            break;
        }

        int64Swap(&pq->data[i], &pq->data[largest]);
        i = largest;
    }
}

// Push a value onto the queue.
bool int64PriorityQueuePush(Int64PriorityQueue *pq, int64_t value) {
    if (pq->size + 1 > pq->capacity) {
        if (!int64PriorityQueueResize(pq, pq->capacity * 2))
            return false;
    }

    pq->data[pq->size] = value; // Add the new value at the end
    heapifyUp(pq, pq->size);    // Restore the max-heap property
    pq->size++;                 // Increase the size of the heap
    return true;
}

// Pop the maximum value from the queue.
int64_t int64PriorityQueuePop(Int64PriorityQueue *pq) {
    if (pq->size == 0) {
        fprintf(stderr, "Error: pop() called on empty priority queue\n");
        exit(EXIT_FAILURE);
    }

    int64_t root = pq->data[0]; // Store the maximum value
    pq->size--;
    if (pq->size > 0) {
        pq->data[0] = pq->data[pq->size]; // Move the last element to the root
        heapifyDown(pq, 0);               // Restore the max-heap property
    }

    return root;
}

// Peek at the maximum value without removing it.
int64_t int64PriorityQueuePeek(Int64PriorityQueue *pq) {
    if (pq->size == 0) {
        fprintf(stderr, "Error: peek() called on empty priority queue\n");
        exit(EXIT_FAILURE);
    }

    return pq->data[0];
}

// Check if the queue is empty.
bool int64PriorityQueueIsEmpty(Int64PriorityQueue *pq) { return pq->size == 0; }

// Get the size of the queue.
size_t int64PriorityQueueSize(Int64PriorityQueue *pq) { return pq->size; }

int main(void) {
    Int64PriorityQueue *pq = int64PriorityQueueCreate(8);
    if (!pq) {
        fprintf(stderr, "Failed to create priority queue\n");
        return EXIT_FAILURE;
    }

    int64PriorityQueuePush(pq, 5);
    int64PriorityQueuePush(pq, 20);
    int64PriorityQueuePush(pq, 110);
    int64PriorityQueuePush(pq, 14);
    int64PriorityQueuePush(pq, -21);
    int64PriorityQueuePush(pq, -84);
    int64PriorityQueuePush(pq, 3);

    printf("The maximum value is: %ld\n", int64PriorityQueuePeek(pq));
    while (!int64PriorityQueueIsEmpty(pq)) {
        int64_t v = int64PriorityQueuePop(pq);
        printf("Popped value: %ld\n", v);
    }
    printf("\n");

    int64PriorityQueueDestroy(pq);
    return EXIT_SUCCESS;
}
