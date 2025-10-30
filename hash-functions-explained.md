# Hash Functions in MemKey: DJB2 and SDBM

## Overview

MemKey uses **double hashing** for collision resolution in its hash table implementation. This technique employs two different hash functions that work together to efficiently handle collisions and ensure good key distribution.

## The Two Hash Functions

### 1. Primary Hash Function: DJB2 🎯

DJB2 (named after Daniel J. Bernstein) is the primary hash function used to determine the initial position for a key.

#### Implementation
```c
static int djb2(char *str, int size) {
    unsigned long hash = 5381;  // Magic number (prime)
    int c;
    while ((c = *str++)) 
        hash = ((hash << 5) + hash) + c;  // hash * 33 + character
    return hash % size;
}
```

#### How DJB2 Works
1. **Initialize** with magic number `5381` (a prime number)
2. **For each character** in the string:
   - Multiply current hash by 33 using bit operations: `(hash << 5) + hash`
   - Add the ASCII value of the character
3. **Return** hash value modulo table size

#### Mathematical Formula
```
hash = 5381
for each character c:
    hash = hash * 33 + c
result = hash % table_size
```

#### Simple Analogy: The Recipe Mixer 🍰
Think of DJB2 like making a cake batter:
1. Start with a **special ingredient** (5381)
2. For each ingredient (character) you add:
   - **Mix vigorously** (multiply by 33)
   - **Add the new ingredient** (+ character value)
3. **Pour into the right-sized pan** (% table size)

#### Example Calculation
Let's trace through hashing the string "hello":

```
Initial: hash = 5381

'h' (ASCII 72):
hash = (5381 * 33) + 72 = 177573 + 72 = 177645

'e' (ASCII 101):
hash = (177645 * 33) + 101 = 5862285 + 101 = 5862386

'l' (ASCII 108):
hash = (5862386 * 33) + 108 = 193458738 + 108 = 193458846

'l' (ASCII 108):
hash = (193458846 * 33) + 108 = 6384141918 + 108 = 6384142026

'o' (ASCII 111):
hash = (6384142026 * 33) + 111 = 210676686858 + 111 = 210676686969

For table size 7: 210676686969 % 7 = 3
```

### 2. Secondary Hash Function: SDBM 🔄

SDBM is the secondary hash function used to calculate the step size for collision resolution.

#### Implementation
```c
static int sdbm(char *str, int size) {
    unsigned long hash = 0;  // Start from 0
    int c;
    while ((c = *str++)) 
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash % size;
}
```

#### How SDBM Works
1. **Initialize** with `0`
2. **For each character**:
   - Take the character value
   - Add `hash << 6` (hash * 64)
   - Add `hash << 16` (hash * 65536)
   - Subtract the original hash
   - This simplifies to: `char + hash * 65599`
3. **Return** hash value modulo table size

#### Mathematical Formula
```
hash = 0
for each character c:
    hash = c + hash * 65599
result = hash % table_size
```

#### Simple Analogy: The Step Counter 👟
Think of SDBM like a pedometer that counts your steps:
1. Start with **0 steps**
2. For each step (character):
   - **Take a big stride** (multiply by 65599)
   - **Add your current position** (+ character)
3. **Fit it to your path length** (% table size)

#### Example Calculation
Let's trace through hashing the string "hello":

```
Initial: hash = 0

'h' (ASCII 72):
hash = 72 + (0 * 65599) = 72

'e' (ASCII 101):
hash = 101 + (72 * 65599) = 101 + 4723128 = 4723229

'l' (ASCII 108):
hash = 108 + (4723229 * 65599) = 108 + 309887346971 = 309887347079

... (numbers get very large)

For table size 7: final_result % 7 = 2
```

## Double Hashing Formula

The two hash functions work together in the double hashing collision resolution:

```c
int hash_func(char *key, int size, int i) {
    int hash = djb2(key, size);           // Primary position
    int res = i == 0 ? hash : hash + (i * sdbm(key, size));
    return res % size;
}
```

Where:
- `i = 0`: First attempt (uses only DJB2)
- `i > 0`: Collision attempts (adds SDBM step size)

## Train Station Analogy 🚂

Imagine you're looking for a seat on a train:

**DJB2 (Primary Hash)** = **Your assigned car number**
- The ticket system assigns you to a specific car
- This is your "first choice" location

**SDBM (Secondary Hash)** = **Your walking pattern**
- If your assigned car is full, SDBM tells you how to move through the train
- It might say "move 2 cars at a time" or "move 4 cars at a time"
- You follow this pattern until you find an empty seat

## Real-World Example

Let's see how both functions work together with a collision scenario:

```
Hash Table Size: 7
Initial state: [null, null, null, null, null, null, null]

Inserting "name":
- DJB2("name") % 7 = 3
- Position 3 is empty → Store at index 3
- Result: [null, null, null, "name", null, null, null]

Inserting "email":
- DJB2("email") % 7 = 3 (COLLISION!)
- SDBM("email") % 7 = 2 (step size)
- Try position: (3 + 1*2) % 7 = 5 → Empty!
- Store at index 5
- Result: [null, null, null, "name", null, "email", null]

Inserting "phone":
- DJB2("phone") % 7 = 3 (COLLISION AGAIN!)
- SDBM("phone") % 7 = 4 (different step size)
- Try position: (3 + 1*4) % 7 = 0 → Empty!
- Store at index 0
- Result: ["phone", null, null, "name", null, "email", null]
```

## Why Two Different Algorithms?

### DJB2 Characteristics
- **Fast**: Simple operations (shift, add, multiply)
- **Good distribution**: Works well for most string inputs
- **Prime-based**: Uses 33 (close to prime) for good mixing
- **Low collision rate**: Produces well-distributed hash values

### SDBM Characteristics
- **Different mathematical approach**: Uses 65599 multiplier
- **Complementary distribution**: Creates different patterns than DJB2
- **Efficient**: Also uses bit operations for speed
- **Step size variety**: Generates diverse step sizes for probing

### Combined Benefits
1. **Unique probe sequences**: Each key gets its own collision path
2. **Reduced clustering**: Keys don't bunch up in the same area
3. **Better space utilization**: More uniform distribution across table
4. **Faster lookups**: Shorter probe sequences on average

## Algorithm Comparison

| Aspect | DJB2 | SDBM |
|--------|------|------|
| **Initial Value** | 5381 | 0 |
| **Multiplier** | 33 | 65599 |
| **Purpose** | Primary position | Step size |
| **Speed** | Very fast | Very fast |
| **Distribution** | Good | Good |
| **Collision Rate** | Low | Varies (used for steps) |

## Performance Benefits

1. **O(1) average case**: Most operations complete in constant time
2. **Good load factor tolerance**: Works well up to 70% capacity
3. **Cache-friendly**: Probing sequence has good locality
4. **Deterministic**: Same key always follows same probe path

## Implementation Notes

- Both functions use **bit shifting** for fast multiplication
- **Prime table sizes** ensure better hash distribution
- **Load factor management** (resize at 70%/10%) prevents degradation
- **Deleted item handling** maintains probe sequence integrity

## Conclusion

The combination of DJB2 and SDBM hash functions provides MemKey with:
- **Efficient collision resolution** through double hashing
- **Good key distribution** across the hash table
- **Fast performance** for both insertion and lookup operations
- **Robust handling** of various string patterns

This dual-function approach makes MemKey's hash table both fast and reliable for Redis-like key-value operations! 🎯