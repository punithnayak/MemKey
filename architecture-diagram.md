# MemKey Architecture Diagram

## System Overview
MemKey is a Redis-like in-memory key-value store with client-server architecture.

```mermaid
graph TB
    subgraph "Client Side"
        CLI[MemKey-cli]
        USER[User Input]
        REPL[REPL Loop]
        PARSER_RESP[Response Parser]
    end
    
    subgraph "Network Layer"
        TCP[TCP/IP Socket<br/>Port 6379]
    end
    
    subgraph "Server Side"
        SERVER[MemKey Server]
        CONN_HANDLER[Connection Handler]
        CMD_PARSER[Command Parser]
        INTERPRETER[Command Interpreter]
        
        subgraph "Core Storage Engine"
            HASHTABLE[Main Hash Table<br/>Key-Value Store]
            
            subgraph "Data Types"
                STRINGS[String Values]
                HASHES[Hash Tables<br/>field:value pairs]
                LISTS[Doubly Linked Lists<br/>head ↔ nodes ↔ tail]
                SETS[Set Collections<br/>unique members]
            end
        end
        
        subgraph "Helper Functions"
            HELPER[Memory Management<br/>Hash Functions<br/>Utilities]
        end
    end
    
    %% User interactions
    USER --> CLI
    CLI --> REPL
    REPL --> TCP
    
    %% Network communication
    TCP <--> CONN_HANDLER
    
    %% Server processing
    CONN_HANDLER --> CMD_PARSER
    CMD_PARSER --> INTERPRETER
    INTERPRETER --> HASHTABLE
    
    %% Data type connections
    HASHTABLE --> STRINGS
    HASHTABLE --> HASHES
    HASHTABLE --> LISTS
    HASHTABLE --> SETS
    
    %% Helper connections
    INTERPRETER --> HELPER
    HASHTABLE --> HELPER
    
    %% Response flow
    INTERPRETER --> CONN_HANDLER
    CONN_HANDLER --> TCP
    TCP --> PARSER_RESP
    PARSER_RESP --> REPL
    
    %% Styling
    classDef client fill:#e1f5fe
    classDef server fill:#f3e5f5
    classDef storage fill:#e8f5e8
    classDef network fill:#fff3e0
    
    class CLI,USER,REPL,PARSER_RESP client
    class SERVER,CONN_HANDLER,CMD_PARSER,INTERPRETER server
    class HASHTABLE,STRINGS,HASHES,LISTS,SETS storage
    class TCP network
```

## Data Flow Architecture

```mermaid
sequenceDiagram
    participant User
    participant CLI as MemKey-cli
    participant Server as MemKey Server
    participant Parser as Command Parser
    participant Interpreter
    participant HashTable as Main HashTable
    participant DataTypes as Data Structures
    
    User->>CLI: Enter command (e.g., "SET key value")
    CLI->>Server: Send command via TCP socket
    Server->>Parser: Parse command string
    Parser->>Parser: Tokenize and validate syntax
    Parser-->>Interpreter: Return Command object
    Interpreter->>HashTable: Execute operation
    HashTable->>DataTypes: Store/retrieve data
    DataTypes-->>HashTable: Return result
    HashTable-->>Interpreter: Return operation result
    Interpreter-->>Server: Format response (Redis protocol)
    Server-->>CLI: Send response via TCP
    CLI->>CLI: Parse response format
    CLI-->>User: Display formatted result
```

## Core Data Structures

```mermaid
classDiagram
    class HashTable {
        +int size
        +int used
        +HashTableItem** items
        +htable_init()
        +htable_set()
        +htable_get()
        +htable_del()
    }
    
    class HashTableItem {
        +enum type
        +char* key
        +void* value
        +STR_T, HASH_T, LIST_T, SET_T
    }
    
    class List {
        +int len
        +ListNode* head
        +ListNode* tail
        +list_lpush()
        +list_rpush()
        +list_lpop()
        +list_rpop()
    }
    
    class ListNode {
        +char* value
        +ListNode* next
        +ListNode* prev
    }
    
    class Set {
        +int size
        +int used
        +char** members
        +set_add()
        +set_rem()
        +set_ismember()
    }
    
    class Command {
        +enum type
        +int argc
        +char** argv
        +SET, GET, HSET, LPUSH, etc.
    }
    
    HashTable ||--o{ HashTableItem : contains
    HashTableItem ||--|| List : "when type=LIST_T"
    HashTableItem ||--|| Set : "when type=SET_T"
    HashTableItem ||--|| HashTable : "when type=HASH_T"
    List ||--o{ ListNode : contains
```

## Command Processing Flow

```mermaid
flowchart TD
    START([Client sends command]) --> PARSE[Command Parser]
    PARSE --> VALIDATE{Valid syntax?}
    VALIDATE -->|No| ERROR[Return error response]
    VALIDATE -->|Yes| INTERPRET[Command Interpreter]
    
    INTERPRET --> CMD_TYPE{Command Type?}
    
    CMD_TYPE -->|String| STR_OPS[String Operations<br/>SET, GET, INCR, etc.]
    CMD_TYPE -->|Hash| HASH_OPS[Hash Operations<br/>HSET, HGET, HDEL, etc.]
    CMD_TYPE -->|List| LIST_OPS[List Operations<br/>LPUSH, RPOP, LRANGE, etc.]
    CMD_TYPE -->|Set| SET_OPS[Set Operations<br/>SADD, SREM, SMEMBERS, etc.]
    CMD_TYPE -->|Key| KEY_OPS[Key Operations<br/>DEL, EXISTS, TYPE]
    CMD_TYPE -->|System| SYS_OPS[System Commands<br/>QUIT, SHUTDOWN]
    
    STR_OPS --> HASHTABLE[Access Main HashTable]
    HASH_OPS --> HASHTABLE
    LIST_OPS --> HASHTABLE
    SET_OPS --> HASHTABLE
    KEY_OPS --> HASHTABLE
    
    HASHTABLE --> RESULT[Generate Response]
    SYS_OPS --> RESULT
    ERROR --> RESULT
    
    RESULT --> FORMAT[Format as Redis Protocol]
    FORMAT --> SEND([Send to Client])
    
    %% Styling
    classDef operation fill:#bbdefb
    classDef storage fill:#c8e6c9
    classDef flow fill:#fff9c4
    
    class STR_OPS,HASH_OPS,LIST_OPS,SET_OPS,KEY_OPS,SYS_OPS operation
    class HASHTABLE storage
    class PARSE,INTERPRET,RESULT,FORMAT flow
```

## Supported Commands by Category

```mermaid
mindmap
  root((MemKey Commands))
    String Commands
      SET/GET
      MSET/MGET
      INCR/DECR
      INCRBY/DECRBY
      STRLEN
    Hash Commands
      HSET/HGET
      HDEL
      HGETALL
      HEXISTS
      HKEYS/HVALS
      HLEN
      HMGET
    List Commands
      LPUSH/RPUSH
      LPOP/RPOP
      LLEN
      LINDEX
      LRANGE
      LSET/LREM
      LPOS
    Set Commands
      SADD/SREM
      SISMEMBER
      SMEMBERS
      SMISMEMBER
    Key Commands
      DEL
      EXISTS
      TYPE
    System Commands
      QUIT
      SHUTDOWN
```

## Memory Layout

```mermaid
graph TD
    subgraph "Memory Organization"
        MAIN_HT[Main Hash Table<br/>Dynamic Array of Pointers]
        
        subgraph "Hash Table Slots"
            SLOT1[Slot 0: key1 → String]
            SLOT2[Slot 1: key2 → Hash Table]
            SLOT3[Slot 2: key3 → List]
            SLOT4[Slot 3: key4 → Set]
            SLOT_N[Slot N: ...]
        end
        
        subgraph "String Storage"
            STR_VAL[char* value]
        end
        
        subgraph "Hash Table Storage"
            NESTED_HT[Nested HashTable<br/>field → value pairs]
        end
        
        subgraph "List Storage"
            LIST_HEAD[ListNode* head] --> NODE1[Node 1] 
            NODE1 --> NODE2[Node 2]
            NODE2 --> NODE3[Node 3]
            LIST_TAIL[ListNode* tail] --> NODE3
        end
        
        subgraph "Set Storage"
            SET_MEMBERS[char** members<br/>Array of strings]
        end
    end
    
    MAIN_HT --> SLOT1
    MAIN_HT --> SLOT2
    MAIN_HT --> SLOT3
    MAIN_HT --> SLOT4
    MAIN_HT --> SLOT_N
    
    SLOT1 --> STR_VAL
    SLOT2 --> NESTED_HT
    SLOT3 --> LIST_HEAD
    SLOT3 --> LIST_TAIL
    SLOT4 --> SET_MEMBERS
```
