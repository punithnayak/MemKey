#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/common.h"

// External declarations for demo purposes
extern HashTableItem HT_DELETED;
extern char SET_DELETED;

// Function declarations
HashTableItem *htable_search(HashTable *ht, char *key);

void print_separator(const char* title) {
    printf("\n========== %s ==========\n", title);
}

void print_hash_table_structure(HashTable *ht) {
    printf("Hash Table Structure:\n");
    printf("├── Size: %d (total slots)\n", ht->size);
    printf("├── Used: %d (occupied slots)\n", ht->used);
    printf("└── Load Factor: %.2f%%\n\n", (float)ht->used / ht->size * 100);
    
    printf("Hash Table Contents:\n");
    for (int i = 0; i < ht->size && ht->used > 0; i++) {
        HashTableItem *item = ht->items[i];
        if (item != NULL && item != &HT_DELETED) {
            printf("Slot[%d]: ", i);
            printf("Key='%s', Type=", item->key);
            
            switch(item->type) {
                case STR_T:
                    printf("STRING, Value='%s'\n", (char*)item->value);
                    break;
                case HASH_T:
                    printf("HASH, Value=HashTable@%p\n", item->value);
                    break;
                case LIST_T:
                    printf("LIST, Value=List@%p\n", item->value);
                    break;
                case SET_T:
                    printf("SET, Value=Set@%p\n", item->value);
                    break;
            }
        }
    }
}

void demonstrate_string_storage(HashTable *ht) {
    print_separator("STRING DATA TYPE STORAGE");
    
    // Store some string values
    htable_set(ht, "name", "Alice");
    htable_set(ht, "counter", "42");
    htable_set(ht, "message", "Hello World!");
    
    printf("String values are stored as simple char* pointers\n\n");
    print_hash_table_structure(ht);
    
    // Show memory addresses
    printf("\nMemory Layout for Strings:\n");
    char *name = htable_get(ht, "name");
    char *counter = htable_get(ht, "counter");
    char *message = htable_get(ht, "message");
    
    printf("name='%s' stored at address: %p\n", name, name);
    printf("counter='%s' stored at address: %p\n", counter, counter);
    printf("message='%s' stored at address: %p\n", message, message);
}

void demonstrate_list_storage(HashTable *ht) {
    print_separator("LIST DATA TYPE STORAGE");
    
    // Create a list
    htable_push(ht, "mylist", "first", LEFT);
    htable_push(ht, "mylist", "second", RIGHT);
    htable_push(ht, "mylist", "third", RIGHT);
    
    // Get the list structure
    HashTableItem *item = htable_search(ht, "mylist");
    List *list = (List*)item->value;
    
    printf("List is stored as a doubly-linked list structure:\n\n");
    printf("List Structure:\n");
    printf("├── Length: %d\n", list->len);
    printf("├── Head pointer: %p\n", list->head);
    printf("└── Tail pointer: %p\n\n", list->tail);
    
    printf("List Nodes (traversing from head to tail):\n");
    ListNode *current = list->head;
    int index = 0;
    while (current != NULL) {
        printf("Node[%d]: ", index);
        printf("Value='%s', Address=%p, Prev=%p, Next=%p\n", 
               current->value, current, current->prev, current->next);
        current = current->next;
        index++;
    }
    
    printf("\nList Memory Layout:\n");
    printf("┌─────────────┐    ┌─────────────┐    ┌─────────────┐\n");
    printf("│    first    │<-->│   second    │<-->│    third    │\n");
    printf("│ prev: NULL  │    │ prev: %p │    │ prev: %p │\n", list->head->next->prev, list->tail->prev);
    printf("│ next: %p │    │ next: %p │    │ next: NULL  │\n", list->head->next, list->tail->prev);
    printf("└─────────────┘    └─────────────┘    └─────────────┘\n");
    printf("      ^                                        ^\n");
    printf("    head                                     tail\n");
}

void demonstrate_hash_storage(HashTable *ht) {
    print_separator("HASH DATA TYPE STORAGE");
    
    // Create a hash (nested hash table)
    htable_hset(ht, "user:1", "name", "Bob");
    htable_hset(ht, "user:1", "age", "25");
    htable_hset(ht, "user:1", "city", "New York");
    
    // Get the nested hash table
    HashTableItem *item = htable_search(ht, "user:1");
    HashTable *nested_ht = (HashTable*)item->value;
    
    printf("Hash is stored as a nested HashTable structure:\n\n");
    printf("Outer Hash Table (main storage):\n");
    printf("Key: 'user:1' -> Points to inner HashTable@%p\n\n", nested_ht);
    
    printf("Inner Hash Table Structure:\n");
    printf("├── Size: %d\n", nested_ht->size);
    printf("├── Used: %d\n", nested_ht->used);
    printf("└── Contents:\n");
    
    for (int i = 0; i < nested_ht->size; i++) {
        HashTableItem *inner_item = nested_ht->items[i];
        if (inner_item != NULL && inner_item != &HT_DELETED) {
            printf("    Slot[%d]: Field='%s' -> Value='%s'\n", 
                   i, inner_item->key, (char*)inner_item->value);
        }
    }
    
    printf("\nHash Memory Layout:\n");
    printf("Main HT['user:1'] --> Nested HT {\n");
    printf("                        ['name'] --> 'Bob'\n");
    printf("                        ['age']  --> '25'\n");
    printf("                        ['city'] --> 'New York'\n");
    printf("                      }\n");
}

void demonstrate_set_storage(HashTable *ht) {
    print_separator("SET DATA TYPE STORAGE");
    
    // Create a set
    htable_sadd(ht, "colors", "red");
    htable_sadd(ht, "colors", "blue");
    htable_sadd(ht, "colors", "green");
    htable_sadd(ht, "colors", "red"); // Duplicate - should be ignored
    
    // Get the set structure
    HashTableItem *item = htable_search(ht, "colors");
    Set *set = (Set*)item->value;
    
    printf("Set is stored as a hash-based array structure:\n\n");
    printf("Set Structure:\n");
    printf("├── Size: %d (total slots)\n", set->size);
    printf("├── Used: %d (unique members)\n", set->used);
    printf("└── Members array: %p\n\n", set->members);
    
    printf("Set Contents (showing hash slots):\n");
    for (int i = 0; i < set->size; i++) {
        char *member = set->members[i];
        if (member != NULL && member != &SET_DELETED) {
            printf("Slot[%d]: '%s' (address: %p)\n", i, member, member);
        } else if (member == &SET_DELETED) {
            printf("Slot[%d]: [DELETED]\n", i);
        } else {
            printf("Slot[%d]: [EMPTY]\n", i);
        }
    }
    
    printf("\nSet guarantees uniqueness through hash-based deduplication\n");
    printf("Member 'red' was added twice but stored only once\n");
}

int main() {
    printf("=== MemKey Data Storage Demonstration ===\n");
    printf("This program shows how different data types are stored in memory\n");
    
    // Initialize the main hash table
    HashTable *ht = htable_init(8);
    
    // Demonstrate each data type
    demonstrate_string_storage(ht);
    demonstrate_list_storage(ht);
    demonstrate_hash_storage(ht);
    demonstrate_set_storage(ht);
    
    print_separator("SUMMARY OF STORAGE PATTERNS");
    printf("1. STRINGS: Stored as char* pointers with strdup() copies\n");
    printf("2. LISTS: Doubly-linked list with head/tail pointers for O(1) push/pop\n");
    printf("3. HASHES: Nested hash tables (hash table within hash table)\n");
    printf("4. SETS: Hash-based array ensuring uniqueness through collision resolution\n\n");
    
    printf("All data types are stored in the main hash table as HashTableItem structures\n");
    printf("Each item contains: {type_enum, key_string, void_pointer_to_actual_data}\n");
    
    print_separator("FINAL MAIN HASH TABLE STATE");
    print_hash_table_structure(ht);
    
    // Cleanup
    htable_free(ht);
    
    return 0;
}