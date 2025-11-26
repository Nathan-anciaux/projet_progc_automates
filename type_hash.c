#include "type_hash.h"
#include "alloc.h"
#include <limits.h>

hash_table *create_hash_table(int element_size) {
    // Allocating memory for the hash table structure.
    hash_table *ht;
    CREATE_MEMORY(ht, 1);

    // Initializing the hash table.
    ht->hash_capacity = 128; // Initial capacity is set to 128.
    ht->hash_size     = 0;   // Initially, the hash table is empty.
    CREATE_MEMORY(ht->hash_table, ht->hash_capacity);

    // Setting all slots to -1 (indicating empty slots).
    for (int i = 0; i < ht->hash_capacity; i++) {
        ht->hash_table[i] = -1;
    }

    // Setting the element size.
    ht->element_size = element_size;
    return ht;
}

void delete_hash_table(hash_table *ht) {
    if (ht == NULL) {
        return;
    }
    free(ht->hash_table);
    free(ht);
}

// Hash function using a simple polynomial rolling hash. Hashes an array of
// integers.
static int
hash_function(int *elements,    //!< The element to hash.
              int size,         //!< Size of the element (number of integers).
              int hash_capacity //!< Capacity of the hash table.
) {
    int hash = 0;
    int a    = 0x9e3779b9; // fractional bits of the golden ratio
    for (int j = 0; j < size; j++) {
        hash = (hash * (size + 1) + (elements[j]) * a) % hash_capacity;
    }
    return hash;
}

// Rehashing function to resize the hash table when load factor exceeds 0.5
static void hash_table_rehash(hash_table *ht, int *elements) {
    // Saving the pointer to the old table and its size.
    int *old_table = ht->hash_table;
    int old_size   = ht->hash_capacity;

    // Doubling the size of the hash table.
    ht->hash_capacity *= 2;
    CREATE_MEMORY(ht->hash_table, ht->hash_capacity);
    for (int i = 0; i < ht->hash_capacity; i++) {
        ht->hash_table[i] = -1;
    }

    // Rehashing the elements from the old table to the new one.
    for (int i = 0; i < old_size; i++) {
        if (old_table[i] == -1) {
            continue; // Skip empty slots
        }
        // Computing the index correspinding to the element with the hash
        // function.
        int index = hash_function(elements + old_table[i] * (ht->element_size),
                                  ht->element_size, ht->hash_capacity);

        // linear probing to find the next available slot
        int j     = 0;
        while (true) {
            int new_index = (index + j) % ht->hash_capacity; // Linear probing
            if (ht->hash_table[new_index] == -1) {
                ht->hash_table[new_index] = old_table[i];
                break; // Successfully inserted
            } else {
                j++;
            }
        }
    }
    free(old_table);
}

// Compares two array of integers to check if they are equal
static bool hash_array_compare(int *a, int *b, int size) {
    for (int i = 0; i < size; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

bool hash_table_insert(hash_table *ht, int *elements, int i, int *ret) {
    // Rehashing if the load factor exceeds 0.5
    if (ht->hash_size >= (ht->hash_capacity >> 1)) {
        hash_table_rehash(ht, elements);
    }

    // Computing the index where the element should be.
    int index = hash_function(elements + i * (ht->element_size),
                              ht->element_size, ht->hash_capacity);

    // Linear probing to find either the element or a free spot
    int s     = 0;
    while (true) {
        int s_index = (index + s) % ht->hash_capacity; // Linear probing
        if (ht->hash_table[s_index] == -1) {
            // If we find an empty spot, we insert the value and return true to
            // indicate that the insertion was performed.
            ht->hash_table[s_index] = i;
            ht->hash_size++;
            return true;
        } else if (hash_array_compare(elements + i * (ht->element_size),
                                      elements + ht->hash_table[s_index] *
                                                     (ht->element_size),
                                      ht->element_size)) {
            if (ret) {
                *ret = ht->hash_table[s_index];
            }
            // If the value is already present, we do not insert it and return
            // false to indicate that the insertion failed.
            return false;
        } else {
            // If the slot is occupied by a different value, we try the next
            // slot.
            s++;
        }
    }
    return false;
}
