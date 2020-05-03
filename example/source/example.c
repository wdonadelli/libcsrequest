#include "libcsrequest.h"

void pquery(csrObject x);



int main(int argc, char *argv[]) {
	char sql[] = "CREATE TABLE tabela (col1 TEXT, col2 NUMBER);";

	new_csr(db, "example.db");
	db.debug(1);
	printf("%d\n", db.sql(sql));

	//if (db.error) printf("Erro: %s\n\n", db.msg);
	//if (!db.add("col1", "adocica meu amor adocia", 0)) {printf("Erro no add");}
	//if (!db.add("col2", "2", 0)) {printf("Erro no add");}
	//if (!db.add("col1", "Música Ruim", 0)) {printf("Erro no add");}
	//if (!db.add("col2", "9999", 0)) {printf("Erro no add");}
	//if (!db.add("col2", "NULL", 1)) {printf("Erro no add");}
	if (!db.add("col1", "Helena", 0)) {printf("Erro no add");}

	db.insert("tabela");	

	printf("%d\n", db.select("tabela"));
	printf("%s\n", db.info());

	//if (!db.select("tabela", pquery)) {
	//	printf("Erro: %s\n\n", db.msg);
	//}
}


void pquery(csrObject x)
{
	printf(
		"row: %ld\nlen: %d\n%s: %s\n%s: %s\n------\n",
		x.row,
		x.len,
		"col1",
		x.fetch("col1"),
		"col2",
		x.fetch("col2")
	);
	
}
