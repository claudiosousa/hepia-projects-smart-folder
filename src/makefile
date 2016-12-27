FLAGS=-g -ggdb -lm -Wall -Wextra -Werror -std=c99 -D_POSIX_SOURCE -D_GNU_SOURCE
LIBS=

searchfolder: main.c ipc.o smart_folder.o parser.o validator.o finder.o linker.o io.o
	gcc $(FLAGS) -o searchfolder main.c *.o $(LIBS)

ipc.o: ipc.c ipc.h
	gcc $(FLAGS) -c ipc.c

smart_folder.o: smart_folder.c smart_folder.h
	gcc $(FLAGS) -c smart_folder.c

parser.o: parser.c parser.h
	gcc $(FLAGS) -c parser.c

validator.o: validator.c validator.h
	gcc $(FLAGS) -c validator.c

finder.o: finder.c finder.h vendor/uthash.h
	gcc $(FLAGS) -c finder.c vendor/uthash.h

linker.o: linker.c linker.h
	gcc $(FLAGS) -c linker.c

io.o: io.c io.h
	gcc $(FLAGS) -c io.c

clean:
	rm *.o vendor/*.gch searchfolder
