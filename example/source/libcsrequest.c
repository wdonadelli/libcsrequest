#include "libcsrequest.h"

/*-----------------------------------------------------------------------------
	Macros
-----------------------------------------------------------------------------*/

/* Erro na alocação de memória */
#define CSR_CHECK_MEMORY(VAR)\
	if (VAR == NULL) {\
		fprintf(\
			stderr,\
			"\n\033[1;31m[%d] libcrequest: Memory allocation error!\033[0m\n\n",\
			__LINE__\
		);\
		exit(1);\
	}\









/*FIXME versão 1*/
/*
static char *ltrim (char *str)
{
	if (str != NULL) {
		while (*str++ <= 33 && *str);
		return --str;
	}
	return "";
}

*/





static char *csrCat(char *str, ...)
{
	/* variáveis locais */
	va_list list;
	char *cat, *txt, *tmp;
	
	/*
		str é o vlor inicial
		cat guardará a string concatenada (será apagada antes de retornar)
		txt retornará o 1º item do array
		tmp guardará os valores de cada entrada
	*/

	/* verificando o primeiro valor */
	if (str == NULL) {
		CSR_SET_STR(cat, "");
		txt = cat;
		free(cat);
		return txt;
	}

	/* atribuindo o primeiro valor */
	CSR_SET_STR(cat, str);

	/* looping nos argumentos */
	va_start(list, str);
	tmp = va_arg(list, char*);

	/* enquanto não encontrar um valor nulo... */
	while (tmp != NULL) {
		cat = realloc (cat, ((strlen(cat) + strlen(tmp) + 1) * sizeof(char)));
		CSR_CHECK_MEMORY(cat);
		strcat(cat, tmp);
		tmp = va_arg(list, char*);
	}
	
	/* Fim do looping */
	va_end(list);
	
	/* definindo ponteiro e limpando memória */
	txt = cat;
	free(cat);

	return txt;
}





























/*---------------------------------------------------------------------------*/

static char *csr_cat (int len, ...)
/* concatena diversas strings ao mesmo tempo */
{
	/* variáveis locais */
	va_list arg;
	char *str, *cat;
	
	/* inicializando variáveis locais */
	str = malloc(sizeof(char));
	CSR_CHECK_MEMORY(str);
	strcpy(str, "");

	/* looping nos argumentos */
	va_start(arg, len);

	for (register int i = 0; i < len; i++) {
		cat = va_arg(arg, char*);
		if (cat == NULL) {cat = csr_cat(0);}
		str = realloc (str, ((strlen(str) + strlen(cat) + 1) * sizeof(char)));
		CSR_CHECK_MEMORY(str);
		strcat(str, cat);
	}
	
	va_end(arg);

	/* retornando ponteiro */
	return str;
}

/*...........................................................................*/

static int callback (void *data, int len, char **val, char **col)
/* função a ser acionada durante pesquisa */
{
	/* variáveis locais: obtendo self */
	csrObject *self = (csrObject*)data;
	
	/* verificando reader */
	if (self->reader == NULL) {
		CSR_WARN("The \"reader\" function is mandatory for queries.", "sql(char *query, void (*reader)())");
		return 1;
	}
	
	/* definindo valores */
	self->row++;
	self->len = len;
	self->col = col;
	self->val = val;
	
	/* executando função */
	self->reader(*self);

	/* retorna 0 se houve sucesso */
	return 0;
}

/*...........................................................................*/

int __csr_sql__ (csrObject *self, char *query, void (*reader)())
{
	printf("---\n%s\n---\n\n", query);
	
	

	char *loko;

	loko = csrCat("willian ", "é ", "muito ", "gostosão! açslkdj çaskldj çaslkdjça slkdjças lkdjçslkdjç alskdjç aslkdjça slkdjça slkdjça slkdjça slkdjçaslkdjça slkdjçaslkdj çaslkdjçaslkdjç aslkdjç lskdjç aslkjdçs lkdjçalskdj çalskdjç slkdjça lskdjças lkdjçaslkd çsjldkj açlskdj 000", NULL);
	printf("size: %ld\n", sizeof(loko));
	printf("frase: %s|\n\n", loko);
	
	

	/* variáveis locais */
	sqlite3 *db;
	int open, exec;
	char *error = NULL;
	
	/* valores iniciais */
	self->error  = 0;
	self->msg    = csr_cat(0);
	self->col    = NULL;
	self->val    = NULL;
	self->row    = 0;
	self->len    = 0;
	self->reader = reader;													\


	/* abrindo/criando banco de dados */
	open = sqlite3_open(self->file, &db);

	/* verificando sucesso no procedimento anterior */
	if (open != SQLITE_OK) {
		self->error = 1;	
		self->msg = csr_cat(1, sqlite3_errmsg(db));
		return 0;
	}
	
	/* executando ação */	
	exec = sqlite3_exec(db, query, callback, self, &error);
	
	/* redefinindo valores */
	self->col    = NULL;
	self->val    = NULL;
	self->row    = 0;
	self->len    = 0;
	self->reader = NULL;	
	self->clear();

	/* verificando sucesso no procedimento anterior */
	if (exec != SQLITE_OK) {
		self->error = 1;	
		self->msg = csr_cat(1, error);
		sqlite3_free(error);
		sqlite3_close(db);
		return 0;
	}

	/* fechando o banco de dados */
	sqlite3_close(db);

	return 1;
}

/*...........................................................................*/

int __csr_insert__ (csrObject *self, char *table)
{
	/* variáveis locais */
	char *col, *val, *query;
	int result;
	csrData *data;

	/* testando informações */
	if (self->data == NULL) {
		CSR_WARN("There is no data to insert (see add method).", "insert(char *table)");
		return 0;
	}

	if (table == NULL || strlen(table) == 0) {
		CSR_WARN("The 'table' argument is mandatory.", "insert(char *table)");
		return 0;
	}

	/* defindo valores iniciais */
	data = malloc(sizeof(csrData));
	CSR_CHECK_MEMORY(data);
	data = self->data;
	col  = csr_cat(0);
	val  = csr_cat(0);

	/* looping: definindo demais valores */
	while (data != NULL) {
		col  = strlen(col) > 0 ? csr_cat(3, col, ", ", data->col) : csr_cat(1, data->col);
		val  = strlen(val) > 0 ? csr_cat(3, val, ", ", data->val) : csr_cat(1, data->val);
		data = data->next;
	}

	/* construindo query */
	query = csr_cat(7, "INSERT INTO ", table, " (", col, ") VALUES (", val, ");");

	/* executando query */
	result = self->sql(query, NULL);

	/* liberando memória */
	free(query);
	free(col);
	free(val);	
	free(data);

	return result;
}

/*...........................................................................*/

int __csr_update__ (csrObject *self, char *table)
{
	/* variáveis locais */
	char *set, *where, *query;
	int result;
	csrData *data;

	/* testando informações */
	if (self->data == NULL) {
		CSR_WARN("There is no data to update (see add method).", "update(char *table)");
		return 0;
	}

	if (table == NULL || strlen(table) == 0) {
		CSR_WARN("The 'table' argument is mandatory.", "update(char *table)");
		return 0;
	}

	/* defindo valores iniciais */
	data = malloc(sizeof(csrData));
	CSR_CHECK_MEMORY(data);
	data  = self->data;
	set   = csr_cat(0);
	where = csr_cat(0);

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (data->where == 1 && strlen(where) == 0) {
			where = csr_cat(4, " WHERE ", data->col, " = ", data->val);
		} else if (strlen(set) > 0) {
			set = csr_cat(5, set, ", ", data->col, " = ", data->val);
		} else {
			set = csr_cat(3, data->col, " = ", data->val);
		}
		data = data->next;
	}

	/* construindo query */
	if (strlen(where) > 0) {
		query = csr_cat(6, "UPDATE ", table, " SET ", set, where, ";");
	} else {
		query = csr_cat(5, "UPDATE ", table, " SET ", set, ";");
	}

	/* executando query */
	result = self->sql(query, NULL);

	/* liberando memória */
	free(query);
	free(set);
	free(where);	
	free(data);

	return result;
}

/*...........................................................................*/

int __csr_delete__ (csrObject *self, char *table)
{
	/* variáveis locais */
	char *where, *query;
	int result;
	csrData *data;

	/* testando informações */
	if (self->data == NULL) {
		CSR_WARN("There is no data to delete (see add method).", "delete(char *table)");
		return 0;
	}

	if (table == NULL || strlen(table) == 0) {
		CSR_WARN("The 'table' argument is mandatory.", "delete(char *table)");
		return 0;
	}

	/* defindo valores iniciais */
	data = malloc(sizeof(csrData));
	CSR_CHECK_MEMORY(data)
	data  = self->data;
	where = csr_cat(0);

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (data->where == 1) {
			where = csr_cat(4, " WHERE ", data->col, " = ", data->val);
			break;
		}
		data = data->next;
	}

	/* construindo query */
	if (strlen(where) > 0) {
		query = csr_cat(4, "DELETE FROM ", table, where, ";");
	} else {
		query = csr_cat(3, "DELETE FROM ", table, ";");
	}

	/* executando query */
	result = self->sql(query, NULL);

	/* liberando memória */
	free(query);
	free(where);	
	free(data);

	return result;
}

/*...........................................................................*/

int __csr_select__ (csrObject *self, char *table, void (*reader)())
{
	/* variáveis locais */
	char *col, *where, *query;
	int result;
	csrData *data;

	/* testando informações */
	if (table == NULL || strlen(table) == 0) {
		CSR_WARN("The 'table' argument is mandatory.", "select(char *table, void (*reader)())");
		return 0;
	}

	/* defindo valores iniciais */
	data = malloc(sizeof(csrData));
	CSR_CHECK_MEMORY(data);
	data  = self->data;
	col   = csr_cat(0);
	where = csr_cat(0);

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (data->where == 1 && strlen(where) == 0) {
			where = csr_cat(4, " WHERE ", data->col, " = ", data->val);
		} else if (strlen(col) > 0) {
			col = csr_cat(3, col, ", ", data->col);
		} else {
			col = csr_cat(1, data->col);
		}
		data = data->next;
	}

	/* construindo query */
	if (strlen(col) == 0) {
		col = csr_cat(1, "*");
	}
	
	if (strlen(where) > 0) {
		query = csr_cat(6, "SELECT ", col, " FROM ", table, where, ";");
	} else {
		query = csr_cat(5, "SELECT ", col, " FROM ", table, ";");
	}

	/* executando query */
	result = self->sql(query, reader);

	/* liberando memória */
	free(query);
	free(col);
	free(where);	
	free(data);

	return result;
}

/*...........................................................................*/

int __csr_add__ (csrObject *self, char *column, char *value, int where)
{
	/* definindo variáveis locais */
	csrData *data;

	/* verificar se a coluna foi informada */
	if (column == NULL || strlen(column) == 0) {
		CSR_WARN("The 'column' argument is mandatory.", "add(char *column, char *value)");
		return 0;
	}

	/* informar sobre a existência de apóstrofo */
	if (strstr(value, "'") != NULL) {
		CSR_INFO("Single quotes must be used in double (''Hi'') in SQLite.", "add(char *column, char *value)");
	}

	/* definindo variáveis */
	data = malloc(sizeof(csrData));
	CSR_CHECK_MEMORY(data);
	data->where = where == 1 ? 1 : 0;
	data->col   = csr_cat(1, column);
	data->val   = value == NULL ? NULL : csr_cat(3, "'", value, "'");

	/* data->next */
	data->next = self->data;
	self->data = data;

	return 1;
}

/*...........................................................................*/

int __csr_clear__ (csrObject *self)
{
	/* definindo variáveis locais */
	csrData *data, *old;
	data = self->data;

	/* looping para liberar as memórias alocadas */
	while (data != NULL) {
		free(data->col);
		free(data->val);
		data->where = 0;
		old  = data;
		data = data->next;
		free(old);
	}

	/* zerando self->data */
	self->data = NULL;

	return 1;
}

/*...........................................................................*/

char *__csr_fetch__ (csrObject *self, char *col)
{
	/* variáveis locais */
	int i;

	/* verificando e redefinindo valores se necessário */
	if (self->col == NULL || self->val == NULL) {
		self->col    = NULL;
		self->val    = NULL;
		self->row    = 0;
		self->len    = 0;
		self->reader = NULL;
		CSR_WARN("Use the method only in queries (SELECT statements)", "fetch(char *col)");
		return NULL;
	}

	/* looping para encontrar coluna */
	for(i = 0; i < self->len; i++){
		if (strcmp(self->col[i], col) == 0) {
			return self->val[i];
		}
	}

	/* se nada for encontrado */
	return NULL;
}



int __csr_path__ (csrObject *self, char *file)
{
	return 1;
}




int __csr__msg__ (csrObject *self, char *msg)
{
	self->msg = malloc(sizeof(char) * (msg == NULL ? 2 : (strlen(msg) + 1)));
	CSR_CHECK_MEMORY(self->msg);
	strcpy(self->msg, (msg == NULL ? "" : msg));
	return 1;
}
