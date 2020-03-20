#include "libcsrequest.h"

int main(int argc, char *argv[]) {

	new_CSrequest(db, "example.db");

	if (!db.sql("CREATE TABLE tabelinha (col4 TEXT);")) {
		printf("Erro: %s\n\n", db.msg);
	}

	if (!db.add("col4", "val 1", 0)) {printf("Erro no add");}
	//if (!db.add("col4", "val 2", 0)) {printf("Erro no add");}
	//if (!db.add("col4", "val 3", 0)) {printf("Erro no add");}
	//if (!db.add("col4", "val 4", 1)) {printf("Erro no add");}

	if (!db.insert("tabelinha")) {
		printf("Erro: %s\n\n", db.msg);
	}

	if (!db.select("tabelinha", NULL)) {
		printf("Erro: %s\n\n", db.msg);
	}

	//db.clear();

	//if (db.add(NULL, NULL)) printf("erro\n"); else printf("sem erro\n");

}
