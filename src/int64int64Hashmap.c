#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_INT64_INT64_HASHMAP_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

// Structure for an entry in the Int64Int64Hashmap.
// Each entry holds an int64_t key, an int64_t value, and a pointer to the next
// entry for handling collisions via separate chaining.
typedef struct Int64Int64HashmapEntry {
    int64_t key;
    int64_t value;
    struct Int64Int64HashmapEntry *next;
} Int64Int64HashmapEntry;

// Structure for the Int64Int64Hashmap.
// It tracks the capacity, the current number of stored entries, and the array
// of buckets.
typedef struct {
    int capacity;
    int size;
    Int64Int64HashmapEntry **buckets;
} Int64Int64Hashmap;

// Simple hash function for int64_t keys.
// It converts negative keys to positive and computes the bucket index using
// modulo.
unsigned int hashInt64(int64_t key, int capacity) {
    if (key < 0)
        key = -key;
    return (unsigned int)(key % capacity);
}

// Dynamically resize the hashmap when the load factor exceeds the threshold.
// This function doubles the capacity, rehashes existing entries, and updates
// the bucket array.
bool resizeInt64Int64Hashmap(Int64Int64Hashmap *map) {
    int oldCapacity = map->capacity;
    int newCapacity = oldCapacity * 2;

    // Allocate a new buckets array.
    Int64Int64HashmapEntry **newBuckets =
        calloc(newCapacity, sizeof(Int64Int64HashmapEntry *));
    if (!newBuckets) {
        fprintf(stderr, "Failed to allocate memory for resizing buckets.\n");
        return false;
    }

    // Rehash every existing entry into the new bucket array.
    for (int i = 0; i < oldCapacity; i++) {
        Int64Int64HashmapEntry *entry = map->buckets[i];
        while (entry) {
            // For every node having the same hash, we need to rehash it one by
            // one into the new buckets.
            Int64Int64HashmapEntry *nextEntry = entry->next;
            unsigned int newIndex = hashInt64(entry->key, newCapacity);
            entry->next = newBuckets[newIndex];
            newBuckets[newIndex] = entry;
            entry = nextEntry;
        }
    }

    // Free the old buckets array and update the map's parameters.
    free(map->buckets);
    map->buckets = newBuckets;
    map->capacity = newCapacity;
    printf("Resized hashmap to new capacity: %d\n", newCapacity);
    return true;
}

// Create and initialize a new Int64Int64Hashmap.
// Returns a pointer to the hashmap if successful, or NULL on failure.
Int64Int64Hashmap *createInt64Int64Hashmap() {
    Int64Int64Hashmap *map = calloc(1, sizeof(Int64Int64Hashmap));
    if (!map) {
        fprintf(stderr, "Failed to allocate memory for Int64Int64Hashmap\n");
        return NULL;
    }

    map->capacity = INITIAL_INT64_INT64_HASHMAP_CAPACITY;
    map->size = 0;
    map->buckets = calloc(map->capacity, sizeof(Int64Int64HashmapEntry *));
    if (!map->buckets) {
        fprintf(stderr, "Failed to allocate memory for buckets\n");
        free(map);
        return NULL;
    }
    return map;
}

// Insert or update a key-value pair in the hashmap.
// Returns true if the operation succeeds, false otherwise.
bool upsertInt64Int64Hashmap(Int64Int64Hashmap *map, int64_t key,
                             int64_t value) {
    if (!map)
        return false;

    // Check load factor; resize if necessary.
    if ((float)(map->size + 1) / map->capacity > LOAD_FACTOR_THRESHOLD) {
        if (!resizeInt64Int64Hashmap(map)) {
            return false;
        }
    }

    unsigned int index = hashInt64(key, map->capacity);
    Int64Int64HashmapEntry *entry = map->buckets[index];

    // If key exists, update its value.
    while (entry) {
        if (entry->key == key) {
            entry->value = value;
            return true;
        }
        entry = entry->next;
    }

    // Key not found; create a new entry.
    Int64Int64HashmapEntry *newEntry =
        calloc(1, sizeof(Int64Int64HashmapEntry));
    if (!newEntry) {
        fprintf(stderr, "Failed to allocate memory for new entry\n");
        return false;
    }

    newEntry->key = key;
    newEntry->value = value;
    newEntry->next = map->buckets[index];
    map->buckets[index] = newEntry;
    map->size++;
    return true;
}

// Retrieve the value associated with a key from the hashmap.
// The retrieved value is stored in the output parameter 'value'.
// Returns true if the key is found, false otherwise.
bool getInt64Int64Hashmap(Int64Int64Hashmap *map, int64_t key, int64_t *value) {
    if (!map || !value)
        return false;
    unsigned int index = hashInt64(key, map->capacity);
    Int64Int64HashmapEntry *entry = map->buckets[index];

    while (entry) {
        if (entry->key == key) {
            *value = entry->value;
            return true;
        }
        entry = entry->next;
    }
    return false;
}

// Remove a key-value pair from the hashmap.
// Returns true if the key was found and removed, false otherwise.
bool removeInt64Int64Hashmap(Int64Int64Hashmap *map, int64_t key) {
    if (!map)
        return false;
    unsigned int index = hashInt64(key, map->capacity);
    Int64Int64HashmapEntry *entry = map->buckets[index];
    Int64Int64HashmapEntry *prev = NULL;

    while (entry) {
        if (entry->key == key) {
            if (prev) {
                // If the target entry is not the first in the list.
                prev->next = entry->next;
            } else {
                // If the target entry is the first in the list.
                map->buckets[index] = entry->next;
            }
            free(entry);
            map->size--;
            return true;
        }
        prev = entry;
        entry = entry->next;
    }
    return false;
}

// Free all memory used by the hashmap.
void freeInt64Int64Hashmap(Int64Int64Hashmap *map) {
    if (!map)
        return;
    for (int i = 0; i < map->capacity; i++) {
        Int64Int64HashmapEntry *entry = map->buckets[i];
        while (entry) {
            Int64Int64HashmapEntry *next = entry->next;
            free(entry);
            entry = next;
        }
    }
    free(map->buckets);
    free(map);
}

// Demonstration of usage.
int main(void) {
    Int64Int64Hashmap *map = createInt64Int64Hashmap();
    if (!map)
        return EXIT_FAILURE;

    // Insert some key-value pairs.
    upsertInt64Int64Hashmap(map, 1, 100);

    // Likely collision with key 1 when capacity is 16.
    upsertInt64Int64Hashmap(map, 17, 1700);
    upsertInt64Int64Hashmap(map, 33, 3300);

    // Retrieve and print values.
    int64_t value;
    if (getInt64Int64Hashmap(map, 1, &value))
        printf("Key 1 => %ld\n", value);
    if (getInt64Int64Hashmap(map, 17, &value))
        printf("Key 17 => %ld\n", value);
    if (getInt64Int64Hashmap(map, 33, &value))
        printf("Key 33 => %ld\n", value);

    // Insert additional keys to trigger a resize.
    for (int64_t i = 2; i <= 20; i++) {
        // Avoid reinserting keys used above.
        if (i == 1 || i == 17 || i == 33)
            continue;
        upsertInt64Int64Hashmap(map, i, i * 100);
    }
    printf("Current capacity after potential resizing: %d\n", map->capacity);

    // Retrieve and print one key after resizing.
    if (getInt64Int64Hashmap(map, 10, &value))
        printf("Key 10 => %ld\n", value);
    else
        printf("Key 10 not found.\n");

    // Remove a key and check.
    if (removeInt64Int64Hashmap(map, 17))
        printf("Key 17 removed successfully.\n");
    else
        printf("Failed to remove key 17.\n");

    // Verify removal.
    if (!getInt64Int64Hashmap(map, 17, &value))
        printf("Key 17 is no longer in the hashmap.\n");

    // Free the hashmap.
    freeInt64Int64Hashmap(map);
    return EXIT_SUCCESS;
}
