#Path to the library
LIB=../libcsrequest
#LIB=source/libcsrequest

#Path to example
MAIN=../example
#MAIN=source/example

all: folder example

example: libcsrequest.o example.o
	@ echo "> Generating executable:"
	gcc -o example objects/example.o objects/libcsrequest.o -l sqlite3
	@ clear
	@ echo "> Running executable:"
	@ echo "================================================================================"
	./example
	@ echo "================================================================================"
	@ echo "> Finished."

libcsrequest.o:
	@ echo "> Compiling libcsrequest.c:"
	gcc -o objects/libcsrequest.o -c $(LIB).c -l sqlite3

example.o:
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
