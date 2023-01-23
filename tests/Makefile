CC = gcc
FLAGS = -Wall -Werror -std=c99 -I ../lib

all: list vec hashmap

list: list.c
	${CC} ${FLAGS} $^ -o $@

vec: vec.c
	${CC} ${FLAGS} $^ -o $@

hashmap: hashmap.c
	${CC} ${FLAGS} $^ -o $@

clean: list vec hashmap
	rm -f list
	rm -f vec
	rm -f hashmap
