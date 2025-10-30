# MemKey Data Storage Examples

Based on the demonstration program output, here's exactly how data gets stored for each data type in MemKey:

## 1. STRING Data Type

**Storage**: Simple `char*` pointers with `strdup()` copies

```
Main Hash Table:
┌─────────────────────────────────────────────────────────┐
│ Slot[2]: Key="name"                                     │
│          ├── type = STR_T                               │
│          ├── key = "name" (strdup copy)                 │
│          └── value = 0x122605f20 ──────────┐            │
└─────────────────────────────────────────────┼────────────┘
                                              │
                                              ▼
                                         ┌─────────┐
                                         │ "Alice" │  (heap allocated string)
                                         └─────────┘
```

**Example Data**:
- `name` → `"Alice"` (stored at 0x122605f20)
- `counter` → `"42"` (stored at 0x122606020) 
- `message` → `"Hello World!"` (stored at 0x122605fa0)

## 2. LIST Data Type

**Storage**: Doubly-linked list with head/tail pointers for O(1) operations

```
Main Hash Table:
┌─────────────────────────────────────────────────────────┐
│ Slot[6]: Key="mylist"                                   │
│          ├── type = LIST_T                              │
│          ├── key = "mylist"                             │
│          └── value = List@0x122605de0 ──────────┐       │
└─────────────────────────────────────────────────┼───────┘
                                                  │
                                                  ▼
                                            ┌─────────────┐
                                            │ List struct │
                                            │ len: 3      │
                                            │ head: ●─────┼──┐
                                            │ tail: ●─────┼─┐│
                                            └─────────────┘ ││
                                                            ││
        ┌───────────────────────────────────────────────────┘│
        │ ┌──────────────────────────────────────────────────┘
        ▼ ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ ListNode    │◄──►│ ListNode    │◄──►│ ListNode    │
│ value:"first"│    │value:"second"│   │value:"third"│
│ prev: NULL  │    │prev: 0x...a0│    │prev: 0x...f0│
│ next: 0x...f0│   │next: 0x...20│    │next: NULL   │
└─────────────┘    └─────────────┘    └─────────────┘
```

**Node Addresses**:
- Node[0]: `"first"` at 0x1226060a0
- Node[1]: `"second"` at 0x1226060f0  
- Node[2]: `"third"` at 0x122606120

## 3. HASH Data Type

**Storage**: Nested hash table (hash table within hash table)

```
Main Hash Table:
┌─────────────────────────────────────────────────────────┐
│ Slot[5]: Key="user:1"                                   │
│          ├── type = HASH_T                              │
│          ├── key = "user:1"                             │
│          └── value = HashTable@0x122704080 ─────────┐   │
└─────────────────────────────────────────────────────┼───┘
                                                      │
                                                      ▼
                                              ┌─────────────────┐
                                              │ Nested HashTable│
                                              │ size: 5         │
                                              │ used: 3         │
                                              │ items[]:        │
                                              │   [0]: age="25" │
                                              │   [1]: [empty]  │
                                              │   [2]: name="Bob"│
                                              │   [3]: city="NY"│
                                              │   [4]: [empty]  │
                                              └─────────────────┘
```

**Field Storage**:
- `user:1.name` → `"Bob"`
- `user:1.age` → `"25"`
- `user:1.city` → `"New York"`

## 4. SET Data Type

**Storage**: Hash-based array ensuring uniqueness through collision resolution

```
Main Hash Table:
┌─────────────────────────────────────────────────────────┐
│ Slot[10]: Key="colors"                                  │
│           ├── type = SET_T                              │
│           ├── key = "colors"                            │
│           └── value = Set@0x122704090 ──────────────┐   │
└─────────────────────────────────────────────────────┼───┘
                                                      │
                                                      ▼
                                              ┌─────────────────┐
                                              │ Set struct      │
                                              │ size: 5         │
                                              │ used: 3         │
                                              │ members[]:      │
                                              │   [0]: "blue"   │
                                              │   [1]: "red"    │
                                              │   [2]: "green"  │
                                              │   [3]: [empty]  │
                                              │   [4]: [empty]  │
                                              └─────────────────┘
```

**Member Storage**:
- `"blue"` at slot[0] (address: 0x122704270)
- `"red"` at slot[1] (address: 0x122704260)
- `"green"` at slot[2] (address: 0x122704220)

## Key Insights

### Memory Layout Pattern
Every data structure follows this pattern in the main hash table:
```c
HashTableItem {
    enum type;           // STR_T, LIST_T, HASH_T, SET_T
    char *key;          // Key string (strdup copy)
    void *value;        // Pointer to actual data structure
}
```

### Type-Specific Storage
1. **Strings**: Direct `char*` pointer to heap-allocated string
2. **Lists**: Pointer to `List` struct containing head/tail pointers
3. **Hashes**: Pointer to nested `HashTable` struct
4. **Sets**: Pointer to `Set` struct with hash-based member array

### Performance Characteristics
- **Hash Tables**: O(1) average lookup with double hashing collision resolution
- **Lists**: O(1) push/pop from both ends, O(n) random access
- **Sets**: O(1) average add/remove/membership testing
- **Dynamic Resizing**: All structures resize when load factor hits 70% (up) or 10% (down)

### Memory Management
- All strings are copied with `strdup()` for data safety
- Proper cleanup with type-aware freeing
- Tombstone deletion markers instead of immediate reorganization