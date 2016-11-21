FLAGS=-g -Wall -Wextra -std=c99 -D_POSIX_SOURCE -iquote./vendor

SRC=../src/

tests: main.c file_validator_test.o $(SRC)ipc.o $(SRC)smart_folder.o $(SRC)file_validator.o $(SRC)finder.o $(SRC)file_linker.o
	gcc $(FLAGS) -o tests main.c *.o $(SRC)*.o

file_validator_test.o: file_validator_test.c
	gcc $(FLAGS) -c file_validator_test.c

clean:
	rm *.o tests

run: tests
	./tests
