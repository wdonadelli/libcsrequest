all: folder example

example: libcsrequest.o example.o
	@ echo "-> example"
	gcc -Wall -o example objects/example.o objects/libcsrequest.o -l sqlite3
	@ echo "--------------------------------------------------------------------"
	./example
	@ echo "--------------------------------------------------------------------"
	@ echo "Done."

libcsrequest.o:
	@ echo "-> libcsrequest.c > libcrequest.o"
	gcc -Wall -o objects/libcsrequest.o -c source/libcsrequest.c -l sqlite3

example.o:
	@ echo "-> example.c > example.o"
	gcc -Wall -o objects/example.o -c source/example.c -l sqlite3

folder:
	@ clear
	@ echo "-> Folders"
	@ cp "../libcsrequest.c" "source/"
	@ cp "../libcsrequest.h" "source/"
	@ mkdir -p objects

clean:
	@ echo "-> Cleaning project"
	@ rm -f example
	@ rm -rf objects/*.o
	@ rm -d objects
	@ echo "Done"
