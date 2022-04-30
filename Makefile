CC=gcc
BIN=index

MAP_SRC:=hashmap.c
INDEX_SRC:=index.c
SET_SRC:=set.c
MAIN_SRC:=main.c

SRC:=common.c ui.c linkedlist.c trie.c $(MAP_SRC) $(INDEX_SRC) $(SET_SRC)
SRC:=$(patsubst %.c,src/%.c, $(SRC))

INCLUDE=include

CFLAGS=-Wall -Wextra -g
LDFLAGS=-lm -lncurses -DLOG_LEVEL=0

all: index


index: src/*.c src/main.c Makefile
	$(info $(SRC))
	$(CC) -o $(BIN) $(CFLAGS) $(SRC) src/main.c -I$(INCLUDE) $(LDFLAGS)

clean:
	rm -rf index test_index
