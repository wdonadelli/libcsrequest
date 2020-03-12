#include "libcsrequest.h"

int main(int argc, char *argv[]) {

	new_CSrequest(db, "example.db");
	printf("file: %s\n\n", db.file);

	db.add("col 1", "val 1", 0);
	db.add("col 2", "val 2", 1);
	db.insert("asd");
	db.clear();

	//if (db.add(NULL, NULL)) printf("erro\n"); else printf("sem erro\n");

}
