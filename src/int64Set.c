#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { EMPTY, OCCUPIED, DELETED } SlotState;

typedef struct {
    int64_t key;
    SlotState state;
} HashSlot;

typedef struct {
    HashSlot *slots;
    size_t size;
    size_t capacity;
} Int64Set;

// A simple hash function for int64_t
static inline uint64_t int64Hash(int64_t key) {
    uint64_t x = (uint64_t)key;
    x = ((x >> 30) ^ x) * 0xbf58476d1ce4e5b9ULL;
    x = ((x >> 27) ^ x) * 0x94d049bb133111ebULL;
    x = (x >> 31) ^ x;
    return x;
}

// Create a new Int64Set with the given capacity.
Int64Set *int64SetCreate(size_t capacity) {
    Int64Set *set = calloc(1, sizeof(Int64Set));
    if (!set) {
        fprintf(stderr, "Failed to allocate memory for Int64Set\n");
        return NULL;
    }

    set->capacity = capacity;
    set->size = 0;
    set->slots = calloc(capacity, sizeof(HashSlot));
    if (!set->slots) {
        fprintf(stderr, "Failed to allocate memory for HashSlot array\n");
        free(set);
        return NULL;
    }

    // Initialize all slots to EMPTY
    for (size_t i = 0; i < capacity; i++) {
        set->slots[i].state = EMPTY;
    }

    return set;
}

// Insert a key into the set.
// Return true if the key is inserted successfully,
// otherwise return false. (including the case that the key already exists)
bool int64SetInsert(Int64Set *set, int64_t key) {
    if (set->size >= set->capacity) {
        fprintf(stderr, "Int64Set is full\n");
        return false;
    }

    size_t index = int64Hash(key) % set->capacity;
    for (size_t i = 0; i < set->capacity; i++) {
        size_t probe = (index + i) % set->capacity;
        if (set->slots[probe].state != OCCUPIED) {
            set->slots[probe].key = key;
            set->slots[probe].state = OCCUPIED;
            set->size++;
            return true;
        } else if (set->slots[probe].state == OCCUPIED &&
                   set->slots[probe].key == key) {
            // If the key already exists in the set,
            // return false and don't insert
            return false;
        }
    }
    return false;
}

// Check if the key exists in the set.
// Return true if the key exists, otherwise return false.
bool int64SetContains(const Int64Set *set, int64_t key) {
    size_t index = int64Hash(key) % set->capacity;
    for (size_t i = 0; i < set->capacity; i++) {
        size_t probe = (index + i) % set->capacity;
        if (set->slots[probe].state == EMPTY) {
            return false;
        } else if (set->slots[probe].state == OCCUPIED &&
                   set->slots[probe].key == key) {
            return true;
        }
    }
    return false;
}

// Destroy the Int64Set and free all memory.
void int64SetDestroy(Int64Set *set) {
    if (set) {
        free(set->slots);
        free(set);
    }
}

int main() {
    Int64Set *set = int64SetCreate(10);
    if (!set) {
        fprintf(stderr, "Failed to create Int64Set\n");
        return 1;
    }

    int64SetInsert(set, 1);
    int64SetInsert(set, 2);
    int64SetInsert(set, 3);

    printf("Contains 1: %d\n", int64SetContains(set, 1));
    printf("Contains 2: %d\n", int64SetContains(set, 2));
    printf("Contains 3: %d\n", int64SetContains(set, 3));
    printf("Contains 4: %d\n", int64SetContains(set, 4));

    int64SetDestroy(set);
    return 0;
}
