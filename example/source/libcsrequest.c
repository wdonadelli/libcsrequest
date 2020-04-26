#include "libcsrequest.h"

/*-----------------------------------------------------------------------------
	Macros
-----------------------------------------------------------------------------*/

/* mensagem padrão */
#define CSR_MSG \
	"\n\033[1;%dm%s[%d] \033[1;36mlibcsrequest > %s\033[0m:\n\t\"%s\"\n\n"

/* mensagem informativa */
#define CSR_INFO(msg, name) \
	printf(CSR_MSG, 34, "INFO", __LINE__, name, msg)

/* mensagem de alerta */
#define CSR_WARN(msg, name) \
	printf(CSR_MSG, 33, "WARN", __LINE__, name, msg)

/* mensagem de erro */
#define CSR_ERROR(msg, name) \
	fprintf(stderr, CSR_MSG, 31, "ERROR", __LINE__, name, msg)

/* verifica se ocorreu erro na alocação de memória */
#define CSR_CHECK_MEMORY(VAR) \
	if (VAR == NULL) { \
		CSR_ERROR("Memory allocation error!", ""); \
		exit(1); \
	}\


/*-----------------------------------------------------------------------------
	Funções internas
-----------------------------------------------------------------------------*/

static int csr_is_empty (char *str)
/* Retorna 1 se a string não possui caracteres imprimíveis */
{
	if (str == NULL) return 1;

	while (*str) {
		/* se for maior que espaço */
		if (*str > 32) return 0;
		str++;
	}
	return 1;
}

/*...........................................................................*/

static int csr_is_name (char *str)
/* Retorna 1 se a string pode ser aceita como nome de coluna/tabela */
{
	if (str == NULL || csr_is_empty(str)) return 0;

	/* se o primeiro elemento for um número */
	if (*str > 47 && *str < 58) return 0;

	while (*str) {
		/* a partir de 0 até 9 */
		if (*str > 47 && *str < 58);
		/* a partir de A até Z */
		else if (*str > 64 && *str < 91);
		/* a partir de a até z */
		else if (*str > 96 && *str < 123);
		/* underline */
		else if (*str == 95);
		/* outros */
		else return 0;
		str++;
	}
	return 1;
}

/*...........................................................................*/

static char *csr_cat(char *str, ...)
/* concatena strings e devolve o ponteiro */
{
	/* variáveis locais */
	va_list list;
	char *cat, *tmp;
	
	/* checando primeiro argumento */
	if (str == NULL) return "";	

	/* definindo variáveis */
	cat = malloc((strlen(str) + 1) * sizeof(char));
	CSR_CHECK_MEMORY(cat);
	strcpy(cat, str);

	/* looping nos argumentos */
	va_start(list, str);
	tmp = va_arg(list, char*);

	/* enquanto não encontrar um valor nulo... */
	while (tmp != NULL) {
		cat = realloc(cat, ((strlen(cat) + strlen(tmp) + 1) * sizeof(char)));
		CSR_CHECK_MEMORY(cat);
		strcat(cat, tmp);
		tmp = va_arg(list, char*);
	}

	/* Fim do looping */
	va_end(list);
	
	return cat;
}

/*...........................................................................*/

static char *csr_set_where (char *col, char *val)
/* define a pesquisa WHERE */
{
	/* variáveis locais */
	char *where;
	where = csr_cat(
		" WHERE ",
		col,
		(val == "NULL" ? " IS " : " = "),
		(val == "NULL" ? "NULL" : val),
		NULL
	);
	return where;
}

/*...........................................................................*/

static void csr_set_msg (csrObject *self, const char *msg)
/* define o valor do atributo msg */
{
	self->msg = malloc((strlen(msg) + 1) * sizeof(char));
	CSR_CHECK_MEMORY(self->msg);
	strcpy(self->msg, msg);
	return;
}

/*...........................................................................*/

static int callback (void *data, int len, char **val, char **col)
/* função a ser acionada durante pesquisa */
{
	/* variáveis locais: obtendo self */
	csrObject *self = (csrObject*)data;
	
	/* verificando reader */
	if (self->reader == NULL) {
		CSR_WARN(
			"The \"reader\" function is mandatory for queries.",
			"sql(char *query, void (*reader)())"
		);
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

/*.............................................................................
	funções de cabeçalho
.............................................................................*/

int __csr_sql__ (csrObject *self, char *query, void (*reader)())
{
	printf("---\n%s\n---\n\n", query);
	
	/* variáveis locais */
	sqlite3 *db;
	int open, exec;
	char *error = NULL;
	
	/* valores iniciais */
	self->error  = 0;
	self->msg    = "";
	self->col    = NULL;
	self->val    = NULL;
	self->row    = 0;
	self->len    = 0;
	self->reader = reader;

	/* abrindo/criando banco de dados */
	open = sqlite3_open(
		csr_is_empty(self->file) ? ":memory:" : self->file,
		&db
	);

	/* verificando sucesso no procedimento anterior */
	if (open != SQLITE_OK) {
		self->error = 1;
		csr_set_msg(self, sqlite3_errmsg(db));
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
		csr_set_msg(self, error);
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
		CSR_WARN(
			"There is no data to insert (see add method).",
			"insert(char *table)"
		);
		return 0;
	}

	if (!csr_is_name(table)) {
		CSR_WARN(
			"Invalid 'table' argument.",
			"insert(char *table)"
		);
		return 0;
	}

	/* defindo valores iniciais */
	data = malloc(sizeof(csrData));
	CSR_CHECK_MEMORY(data);
	data = self->data;
	col  = "";
	val  = "";

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (!data->where) {
			col = csr_cat(
				csr_is_empty(col) ? "" : col,
				csr_is_empty(col) ? "" : ", ",
				data->col,
				NULL
			);
			val = csr_cat(
				csr_is_empty(val) ? "" : val,
				csr_is_empty(val) ? "" : ", ",
				data->val,
				NULL
			);
		}
		data = data->next;
	}

	/* observando resultados */
	if (csr_is_empty(col) || csr_is_empty(val)) {
		CSR_WARN(
			"There is no data to insert (see add method).",
			"insert(char *table)"
		);
		return 0;
	}

	/* construindo query */
	query = csr_cat(
		"INSERT INTO ", table, " (", col, ") VALUES (", val, ");", NULL
	);

	printf("%s\n\n", query);

	/* executando query */
	result = self->sql(query, NULL);

	/* liberando memória */
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
		CSR_WARN(
			"There is no data to update (see add method).",
			"update(char *table)"
		);
		return 0;
	}

	if (!csr_is_name(table)) {
		CSR_WARN(
			"Invalid 'table' argument.",
			"update(char *table)"
		);
		return 0;
	}

	/* defindo valores iniciais */
	data = malloc(sizeof(csrData));
	CSR_CHECK_MEMORY(data);
	data  = self->data;
	set   = "";
	where = "";

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (data->where) {
			where = csr_set_where(data->col, data->val);
		} else {
			set = csr_cat(
				(csr_is_empty(set) ? "" : set),
				(csr_is_empty(set) ? "" : ", "),
				data->col,
				" = ",
				data->val,
				NULL
			);
		}
		data = data->next;
	}

	/* observando resultados */
	if (csr_is_empty(set)) {
		CSR_WARN(
			"There is no data to update (see add method).",
			"update(char *table)"
		);
		return 0;
	}

	if (csr_is_empty(where)) {
		CSR_INFO(
			"The 'WHERE' clause is not defined (see add method).",
			"update(char *table)"
		);
	}

	/* construindo query */
	query = csr_cat("UPDATE ", table, " SET ", set, where, ";", NULL);

	/* executando query */
	result = self->sql(query, NULL);

	/* liberando memória */
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
		CSR_WARN(
			"There is no data to insert (see add method).",
			"delete(char *table)"
		);
		return 0;
	}

	if (!csr_is_name(table)) {
		CSR_WARN(
			"Invalid 'table' argument.",
			"delete(char *table)"
		);
		return 0;
	}

	/* defindo valores iniciais */
	data = malloc(sizeof(csrData));
	CSR_CHECK_MEMORY(data)
	data  = self->data;
	where = "";

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (data->where) {
			where = csr_set_where(data->col, data->val);
		}
		data = data->next;
	}

	/* observando resultados */
	if (csr_is_empty(where)) {
		CSR_INFO(
			"The 'WHERE' clause is not defined (see add method).",
			"delete(char *table)"
		);
	}

	/* construindo query */
	query = csr_cat("DELETE FROM ", table, where, ";", NULL);

	/* executando query */
	result = self->sql(query, NULL);

	/* liberando memória */
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
		if (!csr_is_name(table)) {
		CSR_WARN(
			"Invalid 'table' argument.",
			"select(char *table, void (*reader)())"
		);
		return 0;
	}
	
	/* defindo valores iniciais */
	data = malloc(sizeof(csrData));
	CSR_CHECK_MEMORY(data);
	data  = self->data;
	col   = "";
	where = "";

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (data->where == 1 && strlen(where) == 0) {
			where = csr_cat(" WHERE ", data->col, " = ", data->val, NULL);
		} else if (strlen(col) > 0) {
			col = csr_cat(col, ", ", data->col, NULL);
		} else {
			col = data->col;
		}
		data = data->next;
	}

	/* construindo query */
	query = csr_cat(
		"SELECT ",
		(strlen(col) == 0 ? "*" : col),
		" FROM ", table,
		(strlen(where) > 0 ? where : ""),
		";",
		NULL
	);

	/* executando query */
	result = self->sql(query, reader);

	/* liberando memória */
	free(data);

	return result;
}

/*...........................................................................*/

int __csr_add__ (csrObject *self, char *column, char *value, int where)
{
	/* definindo variáveis locais */
	csrData *data;

	/* verificar se a coluna foi informada */
	if (!csr_is_name(column)) {
		CSR_WARN(
			"Invalid 'column' argument.",
			"add(char *column, char *value)"
		);
		return 0;
	}

	/* informar sobre a existência de apóstrofo */
	if (value != NULL && strstr(value, "'") != NULL) {
		CSR_INFO(
			"Single quotes must be used in double (''Hi'') in SQLite.",
			"add(char *column, char *value)"
		);
	}

	/* definindo variáveis */
	where = where == 1 ? 1 : 0;

	/* verificando se já existe coluna informada e redefinir valores */
	data = self->data;
	while(data) {
		if (data->col == column && data->where == where) {
			data->val = value == NULL ? "NULL" : csr_cat("'", value, "'", NULL);
			break;
		}
		data = data->next;
	}

	/* se	não encontrou coluna, definir nova */
	if (data == NULL) {
		data = malloc(sizeof(csrData));
		CSR_CHECK_MEMORY(data);
		data->where = where;
		data->col   = column;
		data->val   = value == NULL ? "NULL" : csr_cat("'", value, "'", NULL);
		data->next = self->data;
		self->data = data;
	}
	
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
		CSR_WARN(
			"Use the method only in queries (SELECT statements)",
			"fetch(char *col)"
		);
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
