#include "libcsrequest.h"

void pquery(csrObject x);


char *ltrim (char *str)
{
	if (str == NULL) {
		return "";
	}
	while (*str++ <= 33 && *str);
	return --str;
}

char *rtrim (char *str)
{
	if (str == NULL) {
		return "";
	}

	//char txt[(strlen(str)+1)], *old;
	char *txt, *old;
	txt = malloc((strlen(str) + 1) * sizeof(char));
	strcpy(txt, str);
	old = txt + strlen(txt);
	while (*--old <= 33);
	*(old+1) = '\0';
	return txt;
}



char *trim(char *str) {
	return rtrim(ltrim(str));
}






int main(int argc, char *argv[]) {

	char sql[] = "CREATE TABLE tabela (col1 TEXT, col2 NUMBER);";


	char *a = "                  ";
	printf("\n---\nltrim: |%s|\n---\nrtrim: |%s|\n---\ntrim: |%s|\n---\n\n", ltrim(a), rtrim(a), trim(a));











	/* assim funciona com ponteiro, mas precisa do malloc */
/*	printf("----------------------------------\n");
	char *s1 = malloc(8 * sizeof(char));
	//strcpy(s1, "Willian");
	s1 = "WILLIAN";
	printf("s1(%ld): %s\n\n", strlen(s1), s1);
	s1[2] = 'L';
	printf("s1(%ld): %s\n\n", strlen(s1), s1);
	s1[4] = '\0';
	printf("s1(%ld): %s\n\n", strlen(s1), s1);
*/
	/* assim funciona sem ponteiro */
/*	printf("----------------------------------\n");
	char s2[] =  "Willian";
	printf("s2(%ld): %s\n\n", strlen(s2), s2);
	s2[2] = 'L';
	printf("s2(%ld): %s\n\n", strlen(s2), s2);
	s2[4] = '\0';
	printf("s2(%ld): %s\n\n", strlen(s2), s2);
*/
	/* como mudar o valor do ponteiro dentro de outra função? */
/*	printf("----------------------------------\n");
	char *s3 = "donadelli";
	printf("s3(%ld): %s\n\n", strlen(s3), s3);
	change(s3);
	printf("s3(%ld): %s\n\n", strlen(s3), s3);
*/
	
	new_csr(db, "example.db");
	printf("db.file: |%s|\n", db.file);
	printf("db.msg: |%s|\n", db.msg);
	
	
//	printf("db.msg: %s|\n", db.msg);
//	printf("%s\n", trim(""));
	
	//printf("trim: |%s|\n", trim("\t\t\t  xuxu   \t\t\t"));

	

	if (!db.sql(sql)) {
		printf("Erro: %s\n\n", db.msg);
	}

	//db.fetch("loko");

	//if (!db.add("col1", "adocica meu ''amor'' adocia", 0)) {printf("Erro no add");}
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
		"row: %ld\nlen: %d\n%s: %s\n%s: %s\n------\n",
		x.row,
		x.len,
		"col1",
		x.fetch("col1"),
		"col2",
		x.fetch("col2")
	);
	
}
