#Path to the library
LIB=../libcsrequest
#LIB=source/libcsrequest

all: folder main

main: libcsrequest example
	@ echo "> Generating executable:"
	gcc -o example objects/example.o objects/libcsrequest.o
	@ clear
	@ echo "> Running executable:"
	./example
	@ echo "\n> Finished."

libcsrequest:
	@ echo "> Compiling libcsrequest.c:"
	gcc -o objects/libcsrequest.o -c $(LIB).c

example:
	@ echo "> Compiling example.c:"
	gcc -o objects/example.o -c source/example.c

folder:
	@ echo "> Creating objects folder:"
	@ mkdir -p objects

clean:
	@ echo "> Cleaning project:"
	@ rm -rf objects/*.o
	@ rm -f example
	@ rm -d objects
	@ echo "> Finished."
