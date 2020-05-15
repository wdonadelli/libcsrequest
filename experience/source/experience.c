#include "libcsrequest.h"

/* arquivo para testes */

void pquery(csrObject x);

int main(int argc, char *argv[]) {

	char sql[] = "CREATE TABLE IF NOT EXISTS tab (col1 TEXT, col2 NUMBER);";

	new_CSR(db, "experience.db");
	db.sql(sql, NULL);
	db.add("col1", "13", 0);
	db.add("col2", NULL, 0);
	db.add("col1", NULL, 1);
//	db.add("col2", "fgh", 1);
//	db.add("col1", "valor1", 1);
//	db.add("col2", "2", 0);
//	db.add("col2", NULL, 1);
	db.delete("tab");
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
		x.fetch("col22")
	);
}
