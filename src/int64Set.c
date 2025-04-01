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
    float loadFactor;
} Int64Set;

// A simple hash function for int64_t
static inline uint64_t int64Hash(int64_t key) {
    uint64_t x = (uint64_t)key;
    x = ((x >> 30) ^ x) * 0xbf58476d1ce4e5b9ULL;
    x = ((x >> 27) ^ x) * 0x94d049bb133111ebULL;
    x = (x >> 31) ^ x;
    return x;
}

// Resize the set to a new capacity and rehash all keys.
bool int64SetResize(Int64Set *set, size_t newCapacity) {
    HashSlot *oldSlots = set->slots;
    size_t oldCapacity = set->capacity;

    // Allocate new slots and initialize them to EMPTY
    HashSlot *newSlots = calloc(newCapacity, sizeof(HashSlot));
    if (!newSlots) {
        fprintf(stderr, "Failed to allocate memory for new HashSlot array\n");
        return false;
    }
    for (size_t i = 0; i < newCapacity; i++) {
        newSlots[i].state = EMPTY;
    }

    // Rehash all OCCUPIED slots to the new slots
    for (size_t i = 0; i < oldCapacity; i++) {
        if (oldSlots[i].state == OCCUPIED) {
            int64_t key = oldSlots[i].key;
            size_t index = int64Hash(key) % newCapacity;
            for (size_t j = 0; j < newCapacity; j++) {
                size_t probe = (index + j) % newCapacity;
                if (newSlots[probe].state != OCCUPIED) {
                    newSlots[probe].key = key;
                    newSlots[probe].state = OCCUPIED;
                    break;
                }
            }
        }
    }

    // Update the set with the new slots and capacity
    free(oldSlots);
    set->slots = newSlots;
    set->capacity = newCapacity;
    // The size doesn't change after rehashing

    return true;
}

// Create a new Int64Set with the given capacity.
Int64Set *int64SetCreate(size_t capacity, float loadFactor) {
    Int64Set *set = calloc(1, sizeof(Int64Set));
    if (!set) {
        fprintf(stderr, "Failed to allocate memory for Int64Set\n");
        return NULL;
    }

    set->capacity = capacity;
    set->size = 0;
    set->loadFactor = loadFactor;
    if (set->loadFactor <= 0.0f || set->loadFactor >= 1.0f) {
        fprintf(stderr, "Invalid load factor: %f, only accept (0, 1)\n",
                set->loadFactor);
        return NULL;
    }
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
    // Check if the set needs to be resized
    if ((double)(set->size + 1) / set->capacity > set->loadFactor) {
        if (!int64SetResize(set, set->capacity * 2)) {
            // Failed to resize the set
            return false;
        }
    }

    size_t index = int64Hash(key) % set->capacity;
    for (size_t i = 0; i < set->capacity; i++) {
        size_t probe = (index + i) % set->capacity;
        if (set->slots[probe].state != OCCUPIED) {
            // If the slot is empty or deleted, insert the key
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

// Remove a key from the set.
// Return true if the key is removed successfully,
bool int64SetRemove(Int64Set *set, int64_t key) {
    size_t index = int64Hash(key) % set->capacity;
    for (size_t i = 0; i < set->capacity; i++) {
        size_t probe = (index + i) % set->capacity;
        if (set->slots[probe].state == EMPTY) {
            // It's already empty, so the key doesn't exist
            return false;
        } else if (set->slots[probe].state == OCCUPIED &&
                   set->slots[probe].key == key) {
            // If the key is found, mark the slot as DELETED
            set->slots[probe].state = DELETED;
            set->size--;
            return true;
        }
    }
    return false;
}

// Get the current number of keys in the set.
size_t int64SetSize(const Int64Set *set) { return set->size; }

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

// Example usage.
int main() {
    Int64Set *set = int64SetCreate(10, 0.75f);
    if (!set) {
        fprintf(stderr, "Failed to create Int64Set\n");
        return 1;
    }

    // Test 1: Insertion of multiple keys to force resizing.
    printf("=== Inserting keys 0 to 19 ===\n");
    for (int i = 0; i < 20; i++) {
        if (!int64SetInsert(set, i)) {
            printf("Insertion failed for key %d\n", i);
        }
    }
    printf("Set size after inserting 0-19: %zu\n", int64SetSize(set));

    // Test 2: Duplicate insertion.
    printf("\n=== Testing duplicate insertion ===\n");
    if (!int64SetInsert(set, 5))
        printf("Correctly detected duplicate key: 5\n");
    else
        printf("Error: Duplicate key 5 inserted.\n");

    // Test 3: Searching keys (0-24).
    printf("\n=== Checking existence for keys 0 to 24 ===\n");
    for (int i = 0; i < 25; i++) {
        printf("Key %d exists: %d\n", i, int64SetContains(set, i));
    }

    // Test 4: Removing even keys.
    printf("\n=== Removing even keys from 0 to 18 ===\n");
    for (int i = 0; i < 20; i += 2) {
        if (int64SetRemove(set, i))
            printf("Removed key %d successfully.\n", i);
        else
            printf("Failed to remove key %d (or key not found).\n", i);
    }
    printf("Set size after removal: %zu\n", int64SetSize(set));

    // Validate removal.
    printf("\n=== Validating removal ===\n");
    for (int i = 0; i < 20; i++) {
        if (i % 2 == 0) {
            if (int64SetContains(set, i))
                printf("Error: Key %d should have been removed!\n", i);
        } else {
            if (!int64SetContains(set, i))
                printf("Error: Key %d should still be present!\n", i);
        }
    }

    // Test 5: Re-insertion of removed keys.
    printf("\n=== Re-inserting removed even keys (0 to 18) ===\n");
    for (int i = 0; i < 20; i += 2) {
        if (int64SetInsert(set, i))
            printf("Reinserted key %d successfully.\n", i);
        else
            printf("Failed to reinsert key %d.\n", i);
    }
    printf("Set size after reinsertion: %zu\n", int64SetSize(set));

    // Final validation: Check that all keys 0-19 exist.
    printf("\n=== Final validation: Checking keys 0 to 19 ===\n");
    for (int i = 0; i < 20; i++) {
        if (!int64SetContains(set, i))
            printf("Error: Key %d is missing!\n", i);
    }

    int64SetDestroy(set);
    return 0;
}
