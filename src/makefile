FLAGS=-g -Wall -Wextra -std=c99
LIBS=

main: main.c ipc.o smart_folder.o file_validator.o finder.o file_linker.o
	gcc $(FLAGS) -o searchfolder main.c *.o $(LIBS)

ipc.o: ipc.c ipc.h
	gcc $(FLAGS) -c ipc.c

smart_folder.o: smart_folder.c smart_folder.h
	gcc $(FLAGS) -c smart_folder.c

file_validator.o: file_validator.c file_validator.h
	gcc $(FLAGS) -c file_validator.c

finder.o: finder.c finder.h
	gcc $(FLAGS) -c finder.c

file_linker.o: file_linker.c file_linker.h
	gcc $(FLAGS) -c file_linker.c

clean:
	rm *.o searchfolder
