FLAGS=-g -ggdb -lm -Wall -Wextra -Werror -std=c99 -D_POSIX_SOURCE -D_GNU_SOURCE -DDEBUG
LIBS=

searchfolder: main.c ipc.o searchfolder.o parser.o validator.o finder.o linker.o io.o logger.o
	gcc $(FLAGS) -o searchfolder main.c *.o $(LIBS)

ipc.o: ipc.c ipc.h
	gcc $(FLAGS) -c ipc.c

searchfolder.o: searchfolder.c searchfolder.h
	gcc $(FLAGS) -c searchfolder.c

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

logger.o: logger.c logger.h
	gcc $(FLAGS) -c logger.c

clean:
	rm *.o vendor/*.gch searchfolder
