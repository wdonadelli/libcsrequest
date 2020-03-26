#include "libcsrequest.h"

/* Macros */
#define CSR_MSG "\n\033[1;%dm%s[%d] \033[1;36mlibcsrequest > object.%s\033[0m:\n\t\"%s\"\n\n"

#define CSR_INFO(msg, name) printf(CSR_MSG, 34, "INFO", __LINE__, name, msg)
#define CSR_WARN(msg, name) printf(CSR_MSG, 33, "WARN", __LINE__, name, msg)
#define CSR_ERROR(msg, name) fprintf(stderr, CSR_MSG, 31, "ERROR", __LINE__, name, msg)

/*---------------------------------------------------------------------------*/
int csr_empty(char *str)
{
	/* checando se string é nula */
	if (str == NULL) {
		return 1;
	}

	/* varrendo todos os elementos da string */
	for (register int i = 0; i < strlen(str); i++) {
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
			return 0;
		}
	}
	
	return 1;
}
/*---------------------------------------------------------------------------*/

static char *csr_cat(int len, ...)
/* concatena diversas strings ao mesmo tempo */
{
	/* variáveis locais */
	va_list arg;
	char *str, *cat;
	
	/* inicializando variáveis locais */
	str = malloc (sizeof(char));
	if (str == NULL) {
		CSR_ERROR("Memory allocation error (string).", "csr_cat()");
		exit(1);
	}
	strcpy(str, "");

	/* looping nos argumentos */
	va_start(arg, len);

	for (register int i = 0; i < len; i++) {
		cat = va_arg(arg, char*);
		if (cat == NULL) {cat = csr_cat(0);}
		str = realloc (str, ((strlen(str) + strlen(cat) + 1) * sizeof(char)));
		if (str == NULL) {
			CSR_ERROR("Memory allocation error (string).", "csr_cat()");
			exit(1);
		}
		strcat(str, cat);
	}
	
	va_end(arg);

	/* retornando ponteiro */
	return str;
}

/*...........................................................................*/

static int callback(void *data, int len, char **val, char **col)
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
	data = malloc (sizeof(csrData));
	if (data == NULL) {
		CSR_ERROR("Memory allocation error (csrData).", "insert(char *table)");
		exit(1);
	}
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
	data = malloc (sizeof(csrData));
	if (data == NULL) {
		CSR_ERROR("Memory allocation error (csrData).", "update(char *table)");
		exit(1);
	}
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
	data = malloc (sizeof(csrData));
	if (data == NULL) {
		CSR_ERROR("Memory allocation error (csrData).", "delete(char *table)");
		exit(1);
	}
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
	data = malloc (sizeof(csrData));
	if (data == NULL) {
		CSR_ERROR("Memory allocation error (csrData).", "select(char *table, void (*reader)())");
		exit(1);
	}
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
	data = malloc (sizeof(csrData));
	if (data == NULL) {
		CSR_ERROR("Memory allocation error (csrData).", "add(char *column, char *value)");
		exit(1);
	}
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
