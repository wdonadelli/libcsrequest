all: objFolder example

example: objects/example.o objects/libcsrequest.o
	gcc -o example objects/example.o objects/libcsrequest.o

objects/example.o: source/example.c source/libcsrequest.h
	gcc -o objects/example.o -c source/example.c

objects/libcsrequest.o: ./source/libcsrequest.c ./source/libcsrequest.h
	gcc -o objects/libcsrequest.o -c source/libcsrequest.c

objFolder:
	@ mkdir -p objects
