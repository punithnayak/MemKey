# MemKey
MemKey is a high-performance, in-memory key-value store designed to offer fast data access and caching solutions. Built for efficiency, it supports a range of data structures and is ideal for real-time applications requiring low-latency data retrieval.

![Screeenshot of the client program](/Screenshot.png)

## Build Instructions
Instructions to build the project from source.
```bash
# clone this repository
git clone git remote add origin https://github.com/punithnayak/MemKey.git && cd MemKey

# cd into the repository
cd MemKey

# build the redis server (MemKey)
make

# build the redis client (MemKey-cli)
make client


## Usage
`MemKey` is the server's executable, while `MemKey-cli` is the client's. You
can run them separately or use the utility script `connect.sh` to run both at
once (useful when developing).
```bash
# run the utility script
chmod +x connect.sh && ./connect.sh

# run the server
./MemKey

# run the client
./MemKey-cli
```

## Commands supported
```
cmds list:
---
str cmds:        hash cmds:       list cmds:      set cmds:  
- [x] set        - [x] hset       - [x] lpush     - [x] sadd 
- [x] get        - [x] hget       - [x] rpush     - [x] srem 
- [x] mset       - [x] hdel       - [x] lpop      - [x] smembers  
- [x] mget       - [x] hgetall    - [x] rpop      - [x] sismember  
- [x] incr       - [x] hexists    - [x] llen      - [ ] scard  
- [x] decr       - [x] hkeys      - [x] lindex    - [x] smismember
- [x] incrby     - [x] hvals      - [x] lpos      - [ ] sdiff
- [x] decrby     - [x] hlen       - [x] lset      - [ ] sinter
- [x] strlen     - [ ] hincrby    - [x] lrem      - [ ] sunion
- [ ] append     - [x] hmget      - [x] lrange    - [ ] sdiffstore
- [ ] setrange   - [ ] hstrlen    - [ ] lpushx    - [ ] sinterstore
- [ ] getrange   - [ ] hsetnx     - [ ] rpushx    - [ ] sunionstore
- [ ] setnx      - [ ]            - [ ] ltrim     - [ ]
- [ ] msetnx                      - [ ]           
- [ ] 


keys cmds:      etc:
- [x] del       - [ ] ping
- [x] exists    - [x] quit
- [x] type      - [x] shutdown
- [ ] rename    - [ ] 
- [ ]
```


