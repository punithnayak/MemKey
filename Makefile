CC=gcc
CFLAGS=-g -Wall
LDFLAGS=-lm
SRC=$(wildcard src/*.c)
SERVER=$(filter-out src/MemKey-cli.c, $(SRC))
CLIENT=$(filter-out src/MemKey.c, $(SRC))
TEST=$(filter-out src/MemKey.c src/MemKey-cli.c, $(wildcard $(SRC) tests/*.c))

all: server client

server: $(SERVER)
	$(CC) $(CFLAGS) $(SERVER) -o MemKey $(LDFLAGS)

client: $(CLIENT)
	$(CC) $(CFLAGS) $(CLIENT) -o MemKey-cli $(LDFLAGS)

clean:
	rm MemKey* *.out
