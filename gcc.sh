gcc -c libcsrequest.c;
gcc -c example.c;
gcc -o example example.o libcsrequest.o;
./example;
