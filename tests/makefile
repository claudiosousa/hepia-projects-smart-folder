FLAGS=-g -ggdb -lm -Wall -Wextra -std=c99 -D_POSIX_SOURCE
SRC=../src/

tests: parser_test

parser_test: parser_test.c $(SRC)parser.o
	gcc $(FLAGS) -o parser_test parser_test.c $(SRC)parser.o

include $(SRC)makefile

clean:
	rm *_test

run: tests
	./parser_test
