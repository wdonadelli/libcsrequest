all: folder experience

experience: libcsrequest.o experience.o
	@ echo "\033[1;3;96mCompiling experience.o and libcsrequest.o:\033[0m"
	gcc -Wall -g -o experience objects/experience.o objects/libcsrequest.o -l sqlite3
	@ echo "\033[1;3;96mExecute experience:\033[0m"
	./experience
	@ echo "\033[1;3;96mDone!\033[0m"

libcsrequest.o:
	@ echo "\033[1;3;96mCompiling libcsrequest.c:\033[0m"
	gcc -Wall -g -o objects/libcsrequest.o -c source/libcsrequest.c -l sqlite3

experience.o:
	@ echo "\033[1;3;96mCompiling experience.c:\033[0m"
	gcc -Wall -g -o objects/experience.o -c source/experience.c -l sqlite3

folder:
	@ echo "\033[1;3;96mCopying library files to the source directory...\033[0m"
	@ cp "../libcsrequest.c" "source/"
	@ cp "../libcsrequest.h" "source/"

clean:
	@ echo "\033[1;3;96mCleaning up...\033[0m"
	@ rm -f experience
	@ rm -f experience.db
	@ rm -rf objects/*.o
	@ echo "\033[1;3;96mDone!\033[0m"

#see valgrind --tool=memcheck --leak-check=yes ./experience/experience
