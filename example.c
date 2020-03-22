#include "libcsrequest.h"

void pquery(csrObject x);


int main(int argc, char *argv[]) {

	char sql[] = "CREATE TABLE tabela (col1 TEXT, col2 NUMBER);";

	new_csr(db, "example.db");

	

	//if (!db.sql(sql)) {
	//	printf("Erro: %s\n\n", db.msg);
	//}

	//db.fetch("loko");

	if (!db.add("col1", "adocica meu ''amor'' adocia", 0)) {printf("Erro no add");}
	//if (!db.add("col2", "2", 0)) {printf("Erro no add");}

//	if (!db.insert("tabela")) {
	//	printf("Erro: %s\n\n", db.msg);
//	}

	//if (!db.select("tabela", pquery)) {
		//printf("Erro: %s\n\n", db.msg);
//	}
}


void pquery(csrObject x)
{
	printf(
		"row: %d\nlen: %d\n%s: %s\n%s: %s\n------\n",
		x.row,
		x.len,
		"col1",
		x.fetch("col1"),
		"col2",
		x.fetch("col2")
	);
	
}
