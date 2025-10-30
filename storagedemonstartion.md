=== MemKey Data Storage Demonstration ===
This program shows how different data types are stored in memory

========== STRING DATA TYPE STORAGE ==========
String values are stored as simple char* pointers

Hash Table Structure:
├── Size: 11 (total slots)
├── Used: 3 (occupied slots)
└── Load Factor: 27.27%

Hash Table Contents:
Slot[0]: Key='message', Type=STRING, Value='Hello World!'
Slot[2]: Key='name', Type=STRING, Value='Alice'
Slot[4]: Key='counter', Type=STRING, Value='42'

Memory Layout for Strings:
name='Alice' stored at address: 0x122605f20
counter='42' stored at address: 0x122606020
message='Hello World!' stored at address: 0x122605fa0

========== LIST DATA TYPE STORAGE ==========
List is stored as a doubly-linked list structure:

List Structure:
├── Length: 3
├── Head pointer: 0x1226060a0
└── Tail pointer: 0x122606120

List Nodes (traversing from head to tail):
Node[0]: Value='first', Address=0x1226060a0, Prev=0x0, Next=0x1226060f0
Node[1]: Value='second', Address=0x1226060f0, Prev=0x1226060a0, Next=0x122606120
Node[2]: Value='third', Address=0x122606120, Prev=0x1226060f0, Next=0x0

List Memory Layout:
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│    first    │<-->│   second    │<-->│    third    │
│ prev: NULL  │    │ prev: 0x1226060a0 │    │ prev: 0x1226060f0 │
│ next: 0x1226060f0 │    │ next: 0x1226060f0 │    │ next: NULL  │
└─────────────┘    └─────────────┘    └─────────────┘
      ^                                        ^
    head                                     tail

========== HASH DATA TYPE STORAGE ==========
Hash is stored as a nested HashTable structure:

Outer Hash Table (main storage):
Key: 'user:1' -> Points to inner HashTable@0x122704080

Inner Hash Table Structure:
├── Size: 5
├── Used: 3
└── Contents:
    Slot[0]: Field='age' -> Value='25'
    Slot[2]: Field='name' -> Value='Bob'
    Slot[3]: Field='city' -> Value='New York'

Hash Memory Layout:
Main HT['user:1'] --> Nested HT {
                        ['name'] --> 'Bob'
                        ['age']  --> '25'
                        ['city'] --> 'New York'
                      }

========== SET DATA TYPE STORAGE ==========
Set is stored as a hash-based array structure:

Set Structure:
├── Size: 5 (total slots)
├── Used: 3 (unique members)
└── Members array: 0x122704230

Set Contents (showing hash slots):
Slot[0]: 'blue' (address: 0x122704270)
Slot[1]: 'red' (address: 0x122704260)
Slot[2]: 'green' (address: 0x122704220)
Slot[3]: [EMPTY]
Slot[4]: [EMPTY]

Set guarantees uniqueness through hash-based deduplication
Member 'red' was added twice but stored only once

========== SUMMARY OF STORAGE PATTERNS ==========
1. STRINGS: Stored as char* pointers with strdup() copies
2. LISTS: Doubly-linked list with head/tail pointers for O(1) push/pop
3. HASHES: Nested hash tables (hash table within hash table)
4. SETS: Hash-based array ensuring uniqueness through collision resolution

All data types are stored in the main hash table as HashTableItem structures
Each item contains: {type_enum, key_string, void_pointer_to_actual_data}

========== FINAL MAIN HASH TABLE STATE ==========
Hash Table Structure:
├── Size: 11 (total slots)
├── Used: 6 (occupied slots)
└── Load Factor: 54.55%

Hash Table Contents:
Slot[0]: Key='message', Type=STRING, Value='Hello World!'
Slot[2]: Key='name', Type=STRING, Value='Alice'
Slot[4]: Key='counter', Type=STRING, Value='42'
Slot[5]: Key='user:1', Type=HASH, Value=HashTable@0x122704080
Slot[6]: Key='mylist', Type=LIST, Value=List@0x122605de0
Slot[10]: Key='colors', Type=SET, Value=Set@0x122704090