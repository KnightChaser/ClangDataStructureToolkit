#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_INT64_HASHMAP_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

// Structure for an entry in the int64 hashmap.
typedef struct Int64HashmapEntry {
    int64_t key;
    void *value;
    struct Int64HashmapEntry *next;
} Int64HashmapEntry;

// The int64 hashmap structure.
typedef struct {
    int capacity;
    int size;
    Int64HashmapEntry **buckets;
} Int64Hashmap;

// Simple hash function for int64_t keys.
unsigned int hashInt64(int64_t key, int capacity) {
    if (key < 0) {
        key = -key;
    }
    return (unsigned int)(key % capacity);
}

// Dynamically resize the hashmap when the load factor exceeds threshold.
// Thus, we double the capacity and rehash all existing entries.
bool resizeInt64Hashmap(Int64Hashmap *map) {
    int oldCapacity = map->capacity;
    int newCapacity = oldCapacity * 2;

    // Allocate new buckets array.
    Int64HashmapEntry **newBuckets =
        (Int64HashmapEntry **)calloc(newCapacity, sizeof(Int64HashmapEntry *));
    if (!newBuckets) {
        fprintf(stderr, "Failed to allocate memory for resizing buckets.\n");
        return false;
    }

    // Rehash all existing entries into newBuckets.
    for (int i = 0; i < oldCapacity; i++) {
        Int64HashmapEntry *entry = map->buckets[i];
        while (entry) {
            // For each entry having the same index(hash), move it to the new one.
            Int64HashmapEntry *nextEntry = entry->next;
            unsigned int newIndex = hashInt64(entry->key, newCapacity);
            entry->next = newBuckets[newIndex];
            newBuckets[newIndex] = entry;
            entry = nextEntry;
        }
    }

    // Free the old buckets array and update the map.
    free(map->buckets);
    map->buckets = newBuckets;
    map->capacity = newCapacity;

    return true;
}

// Create and initialize a new int64 hashmap.
Int64Hashmap *createInt64Hashmap() {
    Int64Hashmap *hashmap = (Int64Hashmap *)calloc(1, sizeof(Int64Hashmap));
    if (!hashmap) {
        fprintf(stderr, "Failed to allocate memory for Int64Hashmap\n");
        return NULL;
    }
    hashmap->capacity = INITIAL_INT64_HASHMAP_CAPACITY;
    hashmap->size = 0;
    hashmap->buckets = (Int64HashmapEntry **)calloc(
        hashmap->capacity, sizeof(Int64HashmapEntry *));
    if (!hashmap->buckets) {
        fprintf(stderr, "Failed to allocate memory for Int64Hashmap buckets\n");
        free(hashmap);
        return NULL;
    }
    return hashmap;
}

// Insert or update a key-value pair in the int64 hashmap.
// Checks and resizes if the load factor is exceeded.
bool upsertInt64Hashmap(Int64Hashmap *map, int64_t key, void *value) {
    if (!map)
        return false;

    // Check load factor and resize if necessary.
    if ((float)(map->size + 1) / map->capacity > LOAD_FACTOR_THRESHOLD) {
        if (!resizeInt64Hashmap(map)) {
            return false;
        }
    }

    unsigned int index = hashInt64(key, map->capacity);
    Int64HashmapEntry *entry = map->buckets[index];

    // Update the value if the key already exists.
    while (entry) {
        if (entry->key == key) {
            entry->value = value;
            return true;
        }
        entry = entry->next;
    }

    // Key does not exist; create a new entry.
    Int64HashmapEntry *newEntry =
        (Int64HashmapEntry *)calloc(1, sizeof(Int64HashmapEntry));
    if (!newEntry) {
        fprintf(stderr, "Failed to allocate memory for Int64HashmapEntry\n");
        return false;
    }
    newEntry->key = key;
    newEntry->value = value;
    newEntry->next = map->buckets[index];
    map->buckets[index] = newEntry;
    map->size++;
    return true;
}

// Retrieve a value from the int64 hashmap.
// The retrieved value is stored in the output parameter 'value'.
// Returns true if the key is found, false otherwise.
bool getInt64Hashmap(Int64Hashmap *map, int64_t key, void **value) {
    if (!map || !value)
        return false;
    unsigned int index = hashInt64(key, map->capacity);
    Int64HashmapEntry *current = map->buckets[index];
    while (current) {
        if (current->key == key) {
            *value = current->value;
            return true;
        }
        current = current->next;
    }
    return false;
}

// Remove a key from the int64 hashmap.
// Returns true if the key was found and removed, false otherwise.
bool removeInt64Hashmap(Int64Hashmap *map, int64_t key) {
    if (!map)
        return false;
    unsigned int index = hashInt64(key, map->capacity);
    Int64HashmapEntry *current = map->buckets[index];
    Int64HashmapEntry *prev = NULL;
    while (current) {
        if (current->key == key) {
            if (prev) {
                prev->next = current->next;
            } else {
                map->buckets[index] = current->next;
            }
            free(current);
            map->size--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

// Free the memory used by the int64 hashmap.
void freeInt64Hashmap(Int64Hashmap *map) {
    if (!map)
        return;
    for (int i = 0; i < map->capacity; i++) {
        Int64HashmapEntry *current = map->buckets[i];
        while (current) {
            Int64HashmapEntry *next = current->next;
            free(current);
            current = next;
        }
    }
    free(map->buckets);
    free(map);
}

// Example usage demonstrating dynamic resizing.
int main(void) {
    Int64Hashmap *map = createInt64Hashmap();
    if (!map)
        return EXIT_FAILURE;

    // -----------------------
    // Collision Demonstration:
    // -----------------------
    // Keys 1, 17, and 33 all hash to the same index with initial capacity 16.
    int *val1 = malloc(sizeof(int));
    *val1 = 100;
    int *val17 = malloc(sizeof(int));
    *val17 = 1700;
    int *val33 = malloc(sizeof(int));
    *val33 = 3300;

    upsertInt64Hashmap(map, 1, val1);
    upsertInt64Hashmap(map, 17, val17); // Collision with key 1.
    upsertInt64Hashmap(map, 33, val33); // Collision with key 1 and 17.

    printf("Collision Test:\n");
    void *result = NULL;
    if (getInt64Hashmap(map, 1, &result))
        printf("Key 1 => %d\n", *(int *)result);
    if (getInt64Hashmap(map, 17, &result))
        printf("Key 17 => %d\n", *(int *)result);
    if (getInt64Hashmap(map, 33, &result))
        printf("Key 33 => %d\n", *(int *)result);

    // -------------------------
    // Resizing Demonstration:
    // -------------------------
    // Insert additional keys to force a resize.
    // With initial capacity 16 and a threshold of 0.75, adding more than 12
    // entries will trigger a resize.
    printf("\nResizing Test:\n");
    for (int64_t i = 2; i <= 15;
         i++) { // some keys may already exist; skip collisions for simplicity
        // Avoid reinserting keys used in collision demo
        if (i == 1 || i == 17 || i == 33)
            continue;
        int *tempVal = malloc(sizeof(int));
        *tempVal = (int)(i * 10);
        if (!upsertInt64Hashmap(map, i, tempVal)) {
            fprintf(stderr, "Insertion failed for key %ld\n", i);
        }
    }
    // At this point, if the load factor was exceeded, the map should have
    // resized.
    printf("Current capacity after inserting more keys: %d\n", map->capacity);

    // Retrieve one of the keys inserted after resizing.
    if (getInt64Hashmap(map, 10, &result))
        printf("Key 10 => %d\n", *(int *)result);
    else
        printf("Key 10 not found.\n");

    // Cleanup: free all allocated integer values and the hashmap.
    // Note: We free the ones we explicitly allocated.
    free(val1);
    free(val17);
    free(val33);
    // Free additional keys (for simplicity, only keys 2 to 15 except those
    // already freed).
    for (int64_t i = 2; i <= 15; i++) {
        if (i == 1 || i == 17 || i == 33)
            continue;
        void *tempVal = NULL;
        if (getInt64Hashmap(map, i, &tempVal))
            free(tempVal);
    }
    freeInt64Hashmap(map);
    return EXIT_SUCCESS;
}
