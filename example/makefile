#Path to the library
LIB=../libcsrequest
#LIB=source/libcsrequest

#Path to example
MAIN=../example
#MAIN=source/example

all: folder main

main: libcsrequest example
	@ echo "> Generating executable:"
	gcc -o example objects/example.o objects/libcsrequest.o -l sqlite3
	@ clear
	@ echo "> Running executable:"
	./example
	@ echo "\n> Finished."

libcsrequest:
	@ echo "> Compiling libcsrequest.c:"
	gcc -o objects/libcsrequest.o -c $(LIB).c -l sqlite3

example:
	@ echo "> Compiling example.c:"
	gcc -o objects/example.o -c $(MAIN).c -l sqlite3

folder:
	@ echo "> Creating objects folder..."
	@ mkdir -p objects

clean:
	@ echo "> Cleaning project..."
	@ rm -f example
	@ rm -rf objects/*.o
	@ rm -d objects
	@ echo "> Finished."
