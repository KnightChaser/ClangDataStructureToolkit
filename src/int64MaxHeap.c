#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int64_t *data;   // Array to store the heap elements
    size_t size;     // Number of elements in the heap
    size_t capacity; // Maximum number of elements the heap can hold
} Int64MaxHeap;

// Helper function to compute parent/child indices
static inline size_t heapParent(size_t index) { return (index - 1) / 2; }
static inline size_t heapLeftChild(size_t index) { return 2 * index + 1; }
static inline size_t heapRightChild(size_t index) { return 2 * index + 2; }

// Helper function to swap two elements in the heap
static inline void int64Swap(int64_t *a, int64_t *b) {
    int64_t temp = *a;
    *a = *b;
    *b = temp;
}

// Resize underlying array to new capacity, doubling strategy
static bool int64MaxHeapResize(Int64MaxHeap *h, size_t newCapacity) {
    int64_t *tmp = realloc(h->data, newCapacity * sizeof(int64_t));
    if (!tmp) {
        fprintf(stderr, "Failed to resize heap array\n");
        return false;
    }
    h->data = tmp;
    h->capacity = newCapacity;
    return true;
}

// Create a new Int64MaxHeap with the specified initial capacity
Int64MaxHeap *int64MaxHeapCreate(size_t capacity) {
    if (capacity == 0) {
        fprintf(stderr, "Capacity must be greater than 0\n");
        return NULL;
    }
    Int64MaxHeap *h = calloc(1, sizeof(Int64MaxHeap));
    if (!h || !(h->data = calloc(capacity, sizeof(int64_t)))) {
        fprintf(stderr, "Failed to create Int64MaxHeap\n");
        free(h);
        return NULL;
    }
    h->capacity = capacity;
    return h;
}

// Free the Int64MaxHeap memory
void int64MaxHeapDestroy(Int64MaxHeap *h) {
    if (h) {
        free(h->data);
        free(h);
    }
    return;
}

// Restore the max heap property upward from index i
static void heapifyUp(Int64MaxHeap *h, size_t i) {
    while (i > 0) {
        size_t parent = heapParent(i);
        if (h->data[parent] >= h->data[i]) {
            // If the parent is greater or equal, stop
            // because the heap property is satisfied
            break;
        }
        // Otherwise, swap the current node with its parent
        // to maintain the max heap property
        int64Swap(&h->data[parent], &h->data[i]);

        // Now move up to the parent index
        i = parent;
    }
}

// Restore the max heap property downward from index i
static void heapifyDown(Int64MaxHeap *h, size_t i) {
    while (true) {
        size_t leftChild = heapLeftChild(i);
        size_t rightChild = heapRightChild(i);
        size_t largest = i;

        if (leftChild < h->size && h->data[leftChild] > h->data[largest]) {
            // If the left child is greater than the current largest
            largest = leftChild;
        }
        if (rightChild < h->size && h->data[rightChild] > h->data[largest]) {
            // If the right child is greater than the current largest
            largest = rightChild;
        }
        if (largest == i) {
            // If the largest is still the current index, stop
            // because the heap property is satisfied
            break;
        }

        // It means the max heap property is violated
        // (Child is greater than parent)
        if (largest != i) {
            // Swap the current node with the largest child
            int64Swap(&h->data[i], &h->data[largest]);

            // Move down to the largest child's index
            i = largest;
        }
    }
}

// Insert a new value into the heap.
// Return true if successful, false if memory allocation fails.
bool int64MaxHeapInsert(Int64MaxHeap *h, int64_t value) {
    if (h->size + 1 > h->capacity) {
        // Capacity is full, need to resize
        if (!int64MaxHeapResize(h, h->capacity * 2)) {
            return false;
        }
    }

    h->data[h->size] = value;
    h->size++;
    heapifyUp(h, h->size - 1); // -1 because the array is 0-indexed
    return true;
}

// Peek the maximum value in the heap without removing it.
int64_t int64MaxHeapPeek(const Int64MaxHeap *h) {
    if (h->size == 0) {
        fprintf(stderr, "Heap is empty\n");
        exit(EXIT_FAILURE);
    }
    return h->data[0];
}

// Remove and return the maximum value from the heap.
int64_t int64MaxHeapExtract(Int64MaxHeap *h) {
    if (h->size == 0) {
        fprintf(stderr, "Heap is empty\n");
        exit(EXIT_FAILURE);
    }

    int64_t root = h->data[0];
    h->size--;
    h->data[0] = h->data[h->size]; // Move the last element to the root
    heapifyDown(h, 0);             // Restore the max heap property
    return root;
}

bool int64MaxHeapIsEmpty(const Int64MaxHeap *h) { return h->size == 0; }

// Example usage
int main(void) {
    Int64MaxHeap *heap = int64MaxHeapCreate(4);
    if (!heap)
        return 1;

    // Insert some values with alternating signs
    for (int i = 1; i <= 10; i++)
        int64MaxHeapInsert(heap, i * (i % 2 ? 3 : -2));

    // Extract all (equivalent to heap sort!)
    printf("Heap outputs in descending order:\n");
    while (!int64MaxHeapIsEmpty(heap))
        printf("%lld ", (long long)int64MaxHeapExtract(heap));
    printf("\n");

    int64MaxHeapDestroy(heap);
    return 0;
}
