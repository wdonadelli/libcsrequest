all: folder experience

experience: libcsrequest.o experience.o
	@ echo "Compiling experience.o and libcsrequest.o:"
	gcc -Wall -o experience objects/experience.o objects/libcsrequest.o -l sqlite3
	@ echo "Execute experience:"
	./experience
	@ echo "Done!"

libcsrequest.o:
	@ echo "Compiling libcsrequest.c:"
	gcc -Wall -o objects/libcsrequest.o -c source/libcsrequest.c -l sqlite3

experience.o:
	@ echo "Compiling experience.c:"
	gcc -Wall -o objects/experience.o -c source/experience.c -l sqlite3

folder:
	@ echo "Copying library files to the source directory..."
	@ cp "../libcsrequest.c" "source/"
	@ cp "../libcsrequest.h" "source/"

clean:
	@ echo "Cleaning up..."
	@ rm -f experience
	@ rm -f experience.db
	@ rm -rf objects/*.o
	@ echo "Done!"
