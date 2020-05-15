# SQLite Request Library - CSR

SCR is a library written in C language designed to simplify requests with an [SQLite](https://www.sqlite.org/) database.

The interaction with the database occurs through a structure (type) that points to attributes and methods responsible for the requests.

## Handbook

[https://wdonadelli.github.io/libcsrequest/](https://wdonadelli.github.io/libcsrequest/)

## Source Code

- [libcsrequest.h](https://wdonadelli.github.io/libcsrequest/libcsrequest.h)
- [libcsrequest.c](https://wdonadelli.github.io/libcsrequest/libcsrequest.c)

## Package

- *libsqlite3-dev* package or equivalent.

## Compilation (GCC) 

To compile the source code it is necessary to use the `-l sqlite3` flag.

```sh
gcc -c libcsrequest.c -l sqlite3
```

## Versions

### v1.0.0 (2020-05-14)

- Initial release.

## Authors

- Willian Donadelli (<wdonadelli@gmail.com>)
