#include "libcsrequest.h"

int main(int argc, char *argv[]) {

	new_CSrequest(db, "example.db");
	//printf("file: %s\n\n", db.file);

	//db.add("col1", "val 1", 0);
	//db.add("col2", "val 2", 0);
	//db.add("col3", "val 3", 0);
	db.add("col4", "val 4", 1);
	db.add("col4", "val 4", 0);
	//db.add("col5", "val 5", 1);
	//db.add("col6", "val 6", 0);
	//db.add("col7", "val 7", 0);
	//db.add("col8", "val 8", 0);
	//db.add("col9", "val 9", 0);
	//db.add("col0", NULL, 0);
	db.view("tabelinha");
	//db.clear();

	//if (db.add(NULL, NULL)) printf("erro\n"); else printf("sem erro\n");

}
