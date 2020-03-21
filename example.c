#include "libcsrequest.h"

void pquery(csrQuery x);


int main(int argc, char *argv[]) {

	char sql[] = "CREATE TABLE tabela (col1 TEXT, col2 NUMBER);";

	new_csr(db, "example.db");

	if (!db.sql(sql)) {
		printf("Erro: %s\n\n", db.msg);
	}

	if (!db.add("col1", "valor 1", 0)) {printf("Erro no add");}
	if (!db.add("col2", "2", 0)) {printf("Erro no add");}

	if (!db.insert("tabela")) {
		printf("Erro: %s\n\n", db.msg);
	}

	if (!db.select("tabela", pquery)) {
		printf("Erro: %s\n\n", db.msg);
	}
}


void pquery(csrQuery x)
{
	
	printf("col1: %s\ncol2: %s\n\n", x.get("col1"), x.get("col2"));
}
