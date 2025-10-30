# MemKey Command Examples

This document provides comprehensive examples for all commands supported by MemKey. MemKey is a Redis-like in-memory key-value store that supports four main data types: **strings**, **hashes**, **lists**, and **sets**.

## Table of Contents
- [String Commands](#string-commands)
- [Hash Commands](#hash-commands)
- [List Commands](#list-commands)
- [Set Commands](#set-commands)
- [Key Management Commands](#key-management-commands)
- [Control Commands](#control-commands)
- [Error Examples](#error-examples)
- [Practical Usage Examples](#practical-usage-examples)

## String Commands

### `SET` and `GET` - Basic Key-Value Operations
```bash
# Set a string value
SET mykey "hello world"
→ OK

# Get a string value  
GET mykey
→ "hello world"

# Set without a value (empty string)
SET emptykey
→ OK

GET emptykey
→ ""
```

### `MSET` and `MGET` - Multiple Operations
```bash
# Set multiple key-value pairs at once
MSET name "Alice" age "25" city "NYC"
→ OK

# Get multiple values at once
MGET name age city
→ ["Alice", "25", "NYC"]

# MGET with non-existent keys returns null for missing keys
MGET name nonexistent city
→ ["Alice", null, "NYC"]
```

### Numeric Operations
```bash
# Set a number
SET counter "10"
→ OK

# Increment by 1
INCR counter
→ 11

# Decrement by 1  
DECR counter
→ 10

# Increment by specific amount
INCRBY counter 5
→ 15

# Decrement by specific amount
DECRBY counter 3
→ 12

# INCR on non-existent key starts from 0
INCR newcounter
→ 1

# DECR on non-existent key starts from 0
DECR anothercounter
→ -1
```

### String Utilities
```bash
# Get string length
SET message "Hello MemKey!"
STRLEN message
→ 13

# Empty string has length 0
SET empty ""
STRLEN empty
→ 0

# Non-existent key has length 0
STRLEN nonexistent
→ 0
```

## Hash Commands
Hashes store field-value pairs within a single key.

### `HSET`, `HGET` - Basic Hash Operations
```bash
# Set hash fields (can set multiple at once)
HSET user:1001 name "John" email "john@example.com" age "30"
→ 3  # number of fields added

# Get a specific field
HGET user:1001 name
→ "John"

# Get non-existent field
HGET user:1001 phone
→ null

# Update existing field
HSET user:1001 age "31"
→ 0  # 0 because field already existed (updated, not added)
```

### Hash Inspection
```bash
# Get all fields and values
HGETALL user:1001
→ ["name", "John", "email", "john@example.com", "age", "31"]

# Get all field names only
HKEYS user:1001
→ ["name", "email", "age"]

# Get all values only
HVALS user:1001
→ ["John", "john@example.com", "31"]

# Check if field exists
HEXISTS user:1001 email
→ 1  # exists

HEXISTS user:1001 phone
→ 0  # doesn't exist

# Get hash length (number of fields)
HLEN user:1001
→ 3
```

### Multiple Hash Operations
```bash
# Get multiple fields at once
HMGET user:1001 name age phone
→ ["John", "31", null]  # null for non-existent field

# Delete hash fields
HDEL user:1001 age email
→ 2  # number of fields deleted

HGETALL user:1001
→ ["name", "John"]  # only name remains

# Delete non-existent field
HDEL user:1001 nonexistent
→ 0  # 0 fields deleted
```

## List Commands
Lists are ordered collections that support operations at both ends.

### `LPUSH`, `RPUSH` - Adding Elements
```bash
# Push to left (beginning) of list
LPUSH mylist "first"
→ 1  # list length

LPUSH mylist "second" "third"
→ 3  # can push multiple elements

# Current list: ["third", "second", "first"]

# Push to right (end) of list  
RPUSH mylist "fourth"
→ 4

# Current list: ["third", "second", "first", "fourth"]

# Push multiple elements to right
RPUSH mylist "fifth" "sixth"
→ 6

# Current list: ["third", "second", "first", "fourth", "fifth", "sixth"]
```

### `LPOP`, `RPOP` - Removing Elements
```bash
# Pop from left (beginning)
LPOP mylist
→ "third"

# Pop from right (end)
RPOP mylist  
→ "sixth"

# Current list: ["second", "first", "fourth", "fifth"]

# Pop from empty list
LPUSH emptylist "only"
LPOP emptylist
→ "only"
LPOP emptylist
→ null  # empty list
```

### List Inspection and Access
```bash
# Get list length
LLEN mylist
→ 4

# Get element by index (0-based, supports negative indices)
LINDEX mylist 0
→ "second"

LINDEX mylist -1  # last element
→ "fifth"

LINDEX mylist -2  # second to last
→ "fourth"

# Index out of bounds
LINDEX mylist 10
→ null

# Get range of elements (inclusive)
LRANGE mylist 0 1
→ ["second", "first"]

LRANGE mylist 0 -1  # entire list
→ ["second", "first", "fourth", "fifth"]

LRANGE mylist 1 2
→ ["first", "fourth"]

# Range with negative indices
LRANGE mylist -2 -1
→ ["fourth", "fifth"]
```

### List Modification
```bash
# Set element at specific index
LSET mylist 0 "updated_second"
→ OK

LINDEX mylist 0
→ "updated_second"

# Set with negative index
LSET mylist -1 "updated_last"
→ OK

LINDEX mylist -1
→ "updated_last"

# Find position of element
LPOS mylist "first"
→ 1

LPOS mylist "nonexistent"
→ null

# Remove elements by value
LPUSH fruits "apple" "banana" "apple" "cherry" "apple"
# List: ["apple", "cherry", "apple", "banana", "apple"]

LREM fruits 1 "apple"  # remove 1 occurrence from left
→ 1
# List: ["cherry", "apple", "banana", "apple"]

LREM fruits -1 "apple"  # remove 1 occurrence from right
→ 1
# List: ["cherry", "apple", "banana"]

LREM fruits 0 "apple"  # remove all occurrences
→ 1
# List: ["cherry", "banana"]

# Remove non-existent element
LREM fruits 1 "grape"
→ 0  # 0 elements removed
```

## Set Commands
Sets store unique, unordered elements.

### `SADD`, `SREM` - Adding/Removing Elements
```bash
# Add elements to set
SADD myset "apple" "banana" "cherry"
→ 3  # number of elements added

# Add duplicate (won't be added)
SADD myset "apple"
→ 0  # 0 elements added (already exists)

# Add mix of new and existing elements
SADD myset "apple" "date" "elderberry"
→ 2  # only "date" and "elderberry" were new

# Remove elements
SREM myset "banana"
→ 1  # number of elements removed

SREM myset "grape"  # remove non-existent element
→ 0

# Remove multiple elements
SREM myset "apple" "cherry" "fig"
→ 2  # "apple" and "cherry" removed, "fig" didn't exist
```

### Set Inspection
```bash
# Check if element is member
SISMEMBER myset "apple"
→ 0  # not member (was removed)

SISMEMBER myset "date"
→ 1  # is member

# Get all members
SMEMBERS myset
→ ["date", "elderberry"]  # order may vary since sets are unordered

# Check multiple memberships at once
SMISMEMBER myset "date" "apple" "elderberry" "grape"
→ [1, 0, 1, 0]  # corresponding membership status

# Empty set
SMEMBERS emptyset
→ []
```

## Key Management Commands

### Universal Key Operations
```bash
# Delete keys (works on any data type)
SET mystring "hello"
HSET myhash field "value"
LPUSH mylist "item"
SADD myset "element"

DEL mystring myhash
→ 2  # number of keys deleted

# Delete non-existent key
DEL nonexistent
→ 0

# Check if keys exist
EXISTS mylist myset nonexistent
→ 2  # number of existing keys

EXISTS nonexistent1 nonexistent2
→ 0

# Get key type
TYPE mylist
→ "list"

TYPE myset  
→ "set"

TYPE nonexistent
→ "none"

# After deleting all elements, key type becomes "none"
SREM myset "element"
TYPE myset
→ "none"
```

## Control Commands

### Session Management
```bash
# Quit client session
QUIT
→ Connection closes gracefully

# Shutdown server
SHUTDOWN
→ Server shuts down completely
```

## Error Examples

MemKey includes comprehensive error handling:

### Argument Count Errors
```bash
# Wrong argument count
SET
→ ERR wrong number of arguments (given 0, expected 0..2)

GET
→ ERR wrong number of arguments (given 0, expected 1)

HSET mykey
→ ERR wrong number of arguments (given 1, expected 3+)

MSET key1
→ ERR wrong number of arguments (given 1, expected 2+)
```

### Data Type Errors
```bash
# Wrong data type operation
SET mykey "string"
HGET mykey field
→ ERR wrongtype operation

LPUSH mykey "item"
→ ERR wrongtype operation

SADD mykey "member"
→ ERR wrongtype operation

# Create hash then try string operations
HSET myhash field "value"
GET myhash
→ ERR wrongtype operation
```

### Integer/Number Errors
```bash
# Invalid integer operations
SET mykey "not_a_number"
INCR mykey
→ ERR value is not an integer or out of range

INCRBY mykey "also_not_a_number"
→ ERR value is not an integer or out of range

LINDEX mylist "not_an_index"
→ ERR value is not an integer or out of range
```

### Unrecognized Commands
```bash
# Invalid command
INVALIDCMD
→ ERR unrecognized command

WRONGCOMMAND arg1 arg2
→ ERR unrecognized command
```

## Practical Usage Examples

### User Session Management
```bash
# Store user session data
HSET session:abc123 user_id "1001" username "alice" login_time "1635123456" role "admin"
→ 4

# Retrieve session info
HGET session:abc123 username
→ "alice"

HMGET session:abc123 user_id role
→ ["1001", "admin"]

# Check if session exists
EXISTS session:abc123
→ 1

# Set session expiration counter (in seconds)
SET session:abc123:ttl "3600"
→ OK

# Simulate time passing
DECRBY session:abc123:ttl 60  # decrement by 60 seconds
→ 3540

# Check remaining time
GET session:abc123:ttl
→ "3540"

# Clean up expired session
DEL session:abc123 session:abc123:ttl
→ 2
```

### Shopping Cart Implementation
```bash
# Add items to cart (newest items at front)
LPUSH cart:user123 "laptop" "mouse" "keyboard"
→ 3

# View cart contents
LRANGE cart:user123 0 -1
→ ["keyboard", "mouse", "laptop"]

# Remove last added item (undo)
LPOP cart:user123
→ "keyboard"

# Add item to end of cart
RPUSH cart:user123 "monitor"
→ 3

# Current cart: ["mouse", "laptop", "monitor"]

# Get cart size
LLEN cart:user123
→ 3

# Remove specific item
LREM cart:user123 1 "mouse"
→ 1

# View updated cart
LRANGE cart:user123 0 -1
→ ["laptop", "monitor"]

# Clear entire cart
DEL cart:user123
→ 1
```

### Tag System with Sets
```bash
# Add tags to a post
SADD post:100:tags "programming" "tutorial" "redis" "database"
→ 4

# Add tags to another post
SADD post:101:tags "programming" "javascript" "web"
→ 3

# Check if post has specific tag
SISMEMBER post:100:tags "programming"
→ 1

SISMEMBER post:100:tags "javascript"
→ 0

# Get all tags for a post
SMEMBERS post:100:tags
→ ["programming", "tutorial", "redis", "database"]

# Check multiple tags at once
SMISMEMBER post:100:tags "programming" "python" "tutorial" "java"
→ [1, 0, 1, 0]

# Remove a tag
SREM post:100:tags "database"
→ 1

# Add new tag
SADD post:100:tags "nosql"
→ 1
```

### Counter and Statistics
```bash
# Page view counter
SET page:home:views "0"
→ OK

# Increment page views
INCR page:home:views
→ 1

INCR page:home:views
→ 2

# Bulk increment (e.g., from batch processing)
INCRBY page:home:views 50
→ 52

# Multiple counters
MSET page:about:views "10" page:contact:views "5"
→ OK

# Get all counters
MGET page:home:views page:about:views page:contact:views
→ ["52", "10", "5"]

# Calculate total views
# (This would need to be done in application code)
```

### Activity Log
```bash
# Log user activities (most recent first)
LPUSH user:1001:activity "logged_in" "viewed_profile" "updated_settings"
→ 3

# Get recent activities (last 5)
LRANGE user:1001:activity 0 4
→ ["updated_settings", "viewed_profile", "logged_in"]

# Add new activity
LPUSH user:1001:activity "posted_comment"
→ 4

# Trim to keep only last 10 activities (manual implementation)
# Get current length
LLEN user:1001:activity
→ 4

# If length > 10, would need to manually trim using LRANGE and LPUSH to new key
```

### Configuration Storage
```bash
# Store application configuration
HSET app:config db_host "localhost" db_port "5432" cache_ttl "300" debug_mode "true"
→ 4

# Retrieve specific config
HGET app:config debug_mode
→ "true"

# Update configuration
HSET app:config debug_mode "false" log_level "info"
→ 1  # 1 new field added (log_level), debug_mode updated

# Get all configuration
HGETALL app:config
→ ["db_host", "localhost", "db_port", "5432", "cache_ttl", "300", "debug_mode", "false", "log_level", "info"]

# Check what config keys exist
HKEYS app:config
→ ["db_host", "db_port", "cache_ttl", "debug_mode", "log_level"]
```

This comprehensive guide covers all the implemented commands in MemKey with practical examples that demonstrate real-world usage patterns. Each example shows both the command and its expected output, making it easy to understand and test the functionality.