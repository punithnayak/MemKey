# MemKey Internal Data Structures and Command Execution

This document explains what actually happens internally when Redis commands are executed in MemKey, including the data structures that come into existence and how data is stored and managed.

## Table of Contents
- [Overall Architecture](#overall-architecture)
- [Core Data Structures](#core-data-structures)
- [Memory Layout and Storage](#memory-layout-and-storage)
- [Command Execution Flow](#command-execution-flow)
- [Data Structure Examples](#data-structure-examples)
- [Memory Management](#memory-management)

## Overall Architecture

MemKey uses a **hierarchical hash table structure** as its foundation. At the top level, there's a main hash table that maps string keys to various data types (strings, hashes, lists, sets). Each data type has its own specialized internal structure.

```
Main HashTable
├── Key: "user:1001" → HashTable (nested hash table)
├── Key: "shopping_cart" → List (doubly-linked list)
├── Key: "active_users" → Set (hash-based set)
└── Key: "counter" → String (simple string value)
```

## Core Data Structures

### 1. HashTable (Main Container)
```c
typedef struct HashTable {
    int size;           // Current size of the hash table (always prime number)
    int used;           // Number of occupied slots
    HashTableItem **items;  // Array of pointers to hash table items
} HashTable;
```

**Key Features:**
- Uses **double hashing** for collision resolution
- **Dynamic resizing** (grows when >70% full, shrinks when <10% full)
- **Prime number sizing** for better hash distribution

### 2. HashTableItem (Generic Container)
```c
typedef struct HashTableItem {
    enum {STR_T, HASH_T, LIST_T, SET_T} type;  // Data type indicator
    char *key;          // String key
    void *value;        // Pointer to actual data (type depends on 'type')
} HashTableItem;
```

**What happens:** Every Redis key-value pair becomes a `HashTableItem` in the main hash table.

### 3. List (Doubly-Linked List)
```c
typedef struct List {
    int len;            // Current length of the list
    ListNode *head;     // Pointer to first node
    ListNode *tail;     // Pointer to last node
} List;

typedef struct ListNode {
    char *value;        // String value stored in this node
    struct ListNode *next;   // Pointer to next node
    struct ListNode *prev;   // Pointer to previous node
} ListNode;
```

**Used for:** LPUSH, RPUSH, LPOP, RPOP, LRANGE, LINDEX, etc.

### 4. Set (Hash-Based Set)
```c
typedef struct Set {
    int size;           // Current size of the set's internal array
    int used;           // Number of members in the set
    char **members;     // Array of string pointers (hash table without values)
} Set;
```

**Key Features:**
- **Uniqueness guaranteed** through hash-based storage
- **Dynamic resizing** like hash tables
- **Fast membership testing** O(1) average case

## Memory Layout and Storage

### String Storage Example
When you execute: `SET username "alice"`

```
Main HashTable
└── items[hash("username")] → HashTableItem {
    type: STR_T,
    key: "username",           ← strdup("username")
    value: "alice"             ← strdup("alice")
}
```

**Memory allocations:**
1. One `HashTableItem` struct
2. One string copy for the key
3. One string copy for the value

### Hash Storage Example
When you execute: `HSET user:1001 name "John" email "john@email.com"`

```
Main HashTable
└── items[hash("user:1001")] → HashTableItem {
    type: HASH_T,
    key: "user:1001",
    value: → Nested HashTable {
        size: 7,
        used: 2,
        items: [
            [hash("name")] → HashTableItem {
                type: STR_T,
                key: "name",
                value: "John"
            },
            [hash("email")] → HashTableItem {
                type: STR_T,
                key: "email", 
                value: "john@email.com"
            }
        ]
    }
}
```

**Memory allocations:**
1. One `HashTableItem` for the main key
2. One nested `HashTable` structure
3. Two `HashTableItem` structs in the nested table
4. Four string copies (main key + 2 field keys + 2 field values)

### List Storage Example
When you execute: `LPUSH tasks "task1" "task2" "task3"`

```
Main HashTable
└── items[hash("tasks")] → HashTableItem {
    type: LIST_T,
    key: "tasks",
    value: → List {
        len: 3,
        head: → ListNode("task3") ⟷ ListNode("task1") ⟷ ListNode("task2") ← tail
    }
}
```

**Memory allocations:**
1. One `HashTableItem` for the main key
2. One `List` structure
3. Three `ListNode` structures
4. Four string copies (main key + 3 node values)

**Note:** `LPUSH` adds to the head, so the order is reversed from input order.

### Set Storage Example
When you execute: `SADD colors "red" "green" "blue"`

```
Main HashTable
└── items[hash("colors")] → HashTableItem {
    type: SET_T,
    key: "colors",
    value: → Set {
        size: 7,
        used: 3,
        members: [
            [hash("red")] = "red",
            [hash("green")] = "green", 
            [hash("blue")] = "blue"
        ]
    }
}
```

**Memory allocations:**
1. One `HashTableItem` for the main key
2. One `Set` structure
3. One array of string pointers
4. Four string copies (main key + 3 member values)

## Command Execution Flow

### Example: `SET mykey "myvalue"`

1. **Parsing Phase:**
   ```c
   Command cmd = {
       type: SET,
       argc: 2,
       argv: ["mykey", "myvalue"]
   }
   ```

2. **Execution Phase (`exec_set`):**
   ```c
   htable_set(ht, "mykey", "myvalue");
   ```

3. **Hash Table Operations:**
   ```c
   // Check if key exists
   if (htable_exists(ht, "mykey")) {
       // Update existing value
       htable_update_str(ht, "mykey", strdup("myvalue"));
   } else {
       // Insert new item
       htable_insert(ht, STR_T, "mykey", strdup("myvalue"));
   }
   ```

4. **Hash Calculation and Storage:**
   ```c
   for (int i = 0; i < ht->size; i++) {
       int hash = hash_func("mykey", ht->size, i);  // Double hashing
       if (ht->items[hash] == NULL) {
           ht->items[hash] = item_init(STR_T, "mykey", strdup("myvalue"));
           ht->used++;
           break;
       }
   }
   ```

### Example: `LPUSH mylist "item1" "item2"`

1. **First Item (`"item1"`):**
   - Check if `mylist` exists → No
   - Create new `List` structure
   - Create new `ListNode` with value `"item1"`
   - Set `head` and `tail` to this node
   - Insert into main hash table

2. **Second Item (`"item2"`):**
   - Check if `mylist` exists → Yes, type is LIST_T
   - Get existing `List` structure
   - Create new `ListNode` with value `"item2"`
   - Insert at head (LPUSH behavior)
   - Update `head` pointer and link nodes

**Final list structure:**
```
ListNode("item2") ⟷ ListNode("item1")
     ↑                        ↑
   head                     tail
```

### Example: `HSET user:100 name "Alice" age "25"`

1. **Check Key Existence:**
   - Look for `"user:100"` in main hash table → Not found

2. **Create Hash Structure:**
   ```c
   HashTable *new_ht = htable_init(HT_BASE_SIZE);  // Creates nested hash table
   htable_set(new_ht, "name", "Alice");           // Add first field
   htable_set(new_ht, "age", "25");               // Add second field
   htable_insert(ht, HASH_T, "user:100", new_ht); // Insert into main table
   ```

3. **Memory Layout Result:**
   ```
   Main Hash Table
   └── "user:100" → Nested Hash Table
       ├── "name" → "Alice"
       └── "age" → "25"
   ```

## Data Structure Examples

### Complex Nested Example
```bash
# Create user profile with multiple data types
HSET profile:123 name "Alice" status "active"
LPUSH profile:123:sessions "session1" "session2" 
SADD profile:123:permissions "read" "write"
SET profile:123:login_count "5"
```

**Resulting Memory Structure:**
```
Main HashTable
├── "profile:123" → HashTable {
│   ├── "name" → "Alice"
│   └── "status" → "active"
│   }
├── "profile:123:sessions" → List {
│   head → "session2" ⟷ "session1" ← tail
│   }
├── "profile:123:permissions" → Set {
│   members: ["read", "write"]
│   }
└── "profile:123:login_count" → "5"
```

### Hash Table Collision Handling
When multiple keys hash to the same slot:

```c
// Double hashing formula
int hash = (hash1(key) + i * hash2(key)) % table_size;
```

**Example with collisions:**
```
HashTable (size=7)
[0] → NULL
[1] → HashTableItem("key1", "value1")
[2] → NULL  
[3] → HashTableItem("key2", "value2")  // key2 hashed to slot 1, probed to slot 3
[4] → HashTableItem("key3", "value3")  // key3 hashed to slot 1, probed to slot 4
[5] → NULL
[6] → NULL
```

### Dynamic Resizing Example

**Before resize (70% full):**
```
HashTable (size=7, used=5)  // 71% load factor
[0] → item1
[1] → item2
[2] → NULL
[3] → item3
[4] → item4
[5] → item5
[6] → NULL
```

**After resize:**
```
HashTable (size=17, used=5)  // 29% load factor
[0] → NULL
[1] → item1    // Rehashed to new position
[2] → NULL
[3] → item2    // Rehashed to new position
[4] → NULL
[5] → item3    // Rehashed to new position
[6] → NULL
...
[12] → item4   // Rehashed to new position
[13] → NULL
[14] → item5   // Rehashed to new position
[15] → NULL
[16] → NULL
```

## Memory Management

### Automatic Cleanup
MemKey automatically manages memory and cleans up empty data structures:

1. **Empty Lists:** When `LPOP`/`RPOP` removes the last element, the entire list key is deleted
2. **Empty Sets:** When `SREM` removes the last member, the entire set key is deleted  
3. **Empty Hashes:** When `HDEL` removes the last field, the entire hash key is deleted

### Memory Allocation Patterns

**String Operations:**
- Every string value is duplicated (`strdup`) for safety
- Original strings from client input can be freed immediately

**Container Operations:**
- Containers grow dynamically as needed
- Shrink automatically when occupancy drops below 10%

**Nested Structures:**
- Hash tables can contain other hash tables (nested hashes)
- No limit on nesting depth
- Each level manages its own memory

### Memory Footprint Example

For the command: `HSET user:1001 name "John" email "john@example.com" age "25"`

**Memory usage:**
```
Main HashTableItem:          24 bytes (struct + pointers)
Main key string:             10 bytes ("user:1001\0")
Nested HashTable:            16 bytes (struct)
Nested items array:          56 bytes (7 slots × 8 bytes)
Field 1 HashTableItem:       24 bytes
Field 1 key:                 5 bytes ("name\0")
Field 1 value:               5 bytes ("John\0")
Field 2 HashTableItem:       24 bytes  
Field 2 key:                 6 bytes ("email\0")
Field 2 value:               17 bytes ("john@example.com\0")
Field 3 HashTableItem:       24 bytes
Field 3 key:                 4 bytes ("age\0")
Field 3 value:               3 bytes ("25\0")

Total: ~218 bytes (plus malloc overhead)
```

This detailed breakdown shows exactly how MemKey transforms Redis commands into concrete data structures in memory, providing both efficient access patterns and automatic memory management.