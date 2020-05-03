#include "libcsrequest.h"

/*-----------------------------------------------------------------------------
	Macros
-----------------------------------------------------------------------------*/

/* mensagem de saída padrão */
#define CSR_MSG "\n\
\033[0;1;7;%dm %s \
\033[0m \
\033[0;1;3;96mlibcsrequest\
\033[0m.%s\
\033[0;2;3m\n\t%s\
\033[0m\n\n"

/* mensagens de retornos de função */
#define CSR_SELF_DATA_NULL "Parameters not defined (see add method)."
#define CSR_INVALID_TABLE  "Invalid 'table' argument."
#define CSR_BROKEN_SQL     "Inadequate parameters reported (see add method)."
#define CSR_NO_WHERE       "The 'WHERE' clause is not defined (see add method)."
#define CSR_NO_CALLBACK    "The 'reader' function is mandatory for queries."

/* mensagem informativa */
#define CSR_INFO(msg) \
	printf(CSR_MSG, 34, "INFO", csr_prototype, msg)

/* mensagem de alerta (retorna -1) */
#define CSR_WARN(msg) \
	printf(CSR_MSG, 33, "WARN", csr_prototype, msg)

/* mensagem de erro (para a execução) */
#define CSR_ERROR(msg) \
	fprintf(stderr, CSR_MSG, 31, "ERROR", csr_prototype, msg); \
	exit(1)

/* verifica se ocorreu erro na alocação de memória */
#define CSR_CHECK_MEMORY(VAR) \
	if (VAR == NULL) { \
		CSR_ERROR("Memory allocation error: "#VAR); \
	}\

/*-----------------------------------------------------------------------------
	Funções e variáveis internas
-----------------------------------------------------------------------------*/

/* guarda o protótipo da função executada */
static char *csr_prototype = NULL;

/*...........................................................................*/

static int csr_is_empty (const char *str)
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
		(strcmp(val, "NULL") == 0 ? " IS " : " = "),
		(strcmp(val, "NULL") == 0 ? "NULL" : val),
		NULL
	);
	return where;
}

/*...........................................................................*/

static void csr_set_status (csrObject *self, int code, const char *msg)
/* define o valor dos atributos code e message */
{

	/* definindo o status */
	self->code = code == CSR_ERR || code == CSR_FAIL ? code : CSR_OK;

	/* depurando */
	if (self->print && !csr_is_empty(msg)) {
		if (self->code == CSR_OK) {
			CSR_INFO(msg);
		} else {
			CSR_WARN(msg);
		}
	}

	/* definindo a mensagem */
	self->message = malloc((strlen(msg == NULL ? "" : msg) + 1) * sizeof(char));
	CSR_CHECK_MEMORY(self->message);
	strcpy(self->message, msg == NULL ? "" : msg);
	return;
}

/*...........................................................................*/

static void csr_clear_status (csrObject *self)
/* reinicia o status */
{
	csr_set_status(self, CSR_OK, "");
	return;
}

/*...........................................................................*/

static int csr_callback (void *data, int len, char **val, char **col)
/* função a ser acionada durante pesquisa */
{
	/* variáveis locais: obtendo self */
	csrObject *self = (csrObject*)data;
	
	/* verificando reader */
	if (self->reader == NULL) {
		csr_set_status(self, CSR_OK, CSR_NO_CALLBACK);
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
	
	/* protótipo */
	if (csr_prototype == NULL) {
		csr_prototype = "sql (char *query, void (*reader)())";
	}
	
	/* variáveis locais */
	sqlite3 *db;
	int open, exec;
	char *error = NULL;
	
	/* valores iniciais */
	csr_clear_status(self);
	self->col    = NULL;
	self->val    = NULL;
	self->row    = 0;
	self->len    = 0;
	self->reader = reader;

	/* abrindo/criando banco de dados */
	open = sqlite3_open(
		(csr_is_empty(self->file) ? ":memory:" : self->file),
		&db
	);

	/* verificando sucesso no procedimento anterior */
	if (open != SQLITE_OK) {
		csr_set_status(self, CSR_FAIL, sqlite3_errmsg(db));
		return self->status();
	}

	/*-- executando query ----------------------------------------------------*/
	exec = sqlite3_exec(db, query, csr_callback, self, &error);
	/*------------------------------------------------------------------------*/

	/* redefinindo valores */
	self->col     = NULL;
	self->val     = NULL;
	self->row     = 0;
	self->len     = 0;
	self->reader  = NULL;

	/* verificar sucesso no procedimento anterior */
	if (exec != SQLITE_OK) {
		csr_set_status(self, CSR_FAIL, error);
	} else {
		self->clear();
	}

	/* verificando erro ao fechar o banco de dados */
	if (sqlite3_close(db) != SQLITE_OK && self->status() == CSR_OK) {
		csr_set_status(self, CSR_FAIL, error);
	}

	/* liberando memória e zerando csr_prototype */
	sqlite3_free(error);
	csr_prototype = NULL;

	return self->status();
}

/*...........................................................................*/

int __csr_select__ (csrObject *self, char *table, void (*reader)())
{
	/* protótipo */
	csr_prototype = "select (char *table, void (*reader)())";

	/* variáveis locais */
	char *col, *where, *query;
	int result;
	csrData *data;

	/* testando informações */
	if (!csr_is_name(table)) {
		csr_set_status(self, CSR_ERR, CSR_INVALID_TABLE);
		return self->status();
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
		} else {
			col = csr_cat(
				(csr_is_empty(col) ? "" : col),
				(csr_is_empty(col) ? "" : ", "),
				data->col,
				NULL
			);
		}
		data = data->next;
	}

	/* construindo query */
	query = csr_cat(
		"SELECT ",
		(csr_is_empty(col) ? "*" : col),
		" FROM ", table,
		(csr_is_empty(where) ? "" : where),
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

int __csr_insert__ (csrObject *self, char *table)
{
	/* protótipo */
	csr_prototype = "insert (char *table)";

	/* variáveis locais */
	char *col, *val, *query;
	int result;
	csrData *data;

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

	/* verificando erros */
	if (self->data == NULL) {
		csr_set_status(self, CSR_ERR, CSR_SELF_DATA_NULL);
	} else if (!csr_is_name(table)) {
		csr_set_status(self, CSR_ERR, CSR_INVALID_TABLE);
	} else if (csr_is_empty(col) || csr_is_empty(val)) {
		csr_set_status(self, CSR_ERR, CSR_BROKEN_SQL);
	} else {
		csr_clear_status(self);
	}
	if (self->status() != CSR_OK) return self->status();
	

	/* construindo query */
	query = csr_cat(
		"INSERT INTO ", table, " (", col, ") VALUES (", val, ");", NULL
	);

	/* executando query */
	result = self->sql(query, NULL);

	/* liberando memória */
	free(data);

	return result;
}

/*...........................................................................*/

int __csr_update__ (csrObject *self, char *table)
{
	/* prototype */
	csr_prototype = "update (char *table)";

	/* variáveis locais */
	char *set, *where, *query;
	int result;
	csrData *data;

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

	/* verificando erros */
	if (self->data == NULL) {
		csr_set_status(self, CSR_ERR, CSR_SELF_DATA_NULL);
	} else if (!csr_is_name(table)) {
		csr_set_status(self, CSR_ERR, CSR_INVALID_TABLE);
	} else if (csr_is_empty(set)) {
		csr_set_status(self, CSR_ERR, CSR_BROKEN_SQL);
	} else if (csr_is_empty(where)) {
		csr_set_status(self, CSR_ERR, CSR_NO_WHERE);
	} else {
		csr_clear_status(self);
	}
	if (self->status() != CSR_OK) return self->status();

	/* construindo query */
	query = csr_cat(
		"UPDATE ", table, " SET ", set, where, ";", NULL
	);

	/* executando query */
	result = self->sql(query, NULL);

	/* liberando memória */
	free(data);

	return result;
}

/*...........................................................................*/

int __csr_delete__ (csrObject *self, char *table)
{
	/* protótipo */
	csr_prototype = "delete (char *table)";

	/* variáveis locais */
	char *where, *query;
	int result;
	csrData *data;

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

	/* verificando erros */
	if (self->data == NULL) {
		csr_set_status(self, CSR_ERR, CSR_SELF_DATA_NULL);
	} else if (!csr_is_name(table)) {
		csr_set_status(self, CSR_ERR, CSR_INVALID_TABLE);
	} else if (csr_is_empty(where)) {
		csr_set_status(self, CSR_ERR, CSR_NO_WHERE);
	} else {
		csr_clear_status(self);
	}
	if (self->status() != CSR_OK) return self->status();

	/* construindo query */
	query = csr_cat(
		"DELETE FROM ", table, where, ";", NULL
	);

	/* executando query */
	result = self->sql(query, NULL);

	/* liberando memória */
	free(data);

	return result;
}

/*...........................................................................*/

int __csr_add__ (csrObject *self, char *column, char *value, int where)
{
	/* protótipo */
	csr_prototype = "add (char *column, char *value)";

	/* definindo variáveis locais */
	csrData *data;

	/* verificar se a coluna foi informada */
	if (!csr_is_name(column)) {
		CSR_WARN("Invalid 'column' argument.");
		return 0;
	}

	/* informar sobre a existência de apóstrofo */
	if (value != NULL && strstr(value, "'") != NULL) {
		CSR_INFO("Single quotes must be used in double (''Hi'') in SQLite.");
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
	/* protótipo */
	csr_prototype = "clear ()";

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
	/* protótipo */
	csr_prototype = "fetch (char *col)";

	/* variáveis locais */
	int i;

	/* verificando e redefinindo valores se necessário */
	if (self->col == NULL || self->val == NULL) {
		self->col    = NULL;
		self->val    = NULL;
		self->row    = 0;
		self->len    = 0;
		self->reader = NULL;
/*
		CSR_WARN2(
			"Use the method only in queries (SELECT statements)",
			"fetch(char *col)"
		);
*/
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

/*...........................................................................*/

int __csr_status__ (csrObject *self)
{
	return self->code;
}

/*...........................................................................*/

char *__csr_info__ (csrObject *self)
{
	return self->message;
}

/*...........................................................................*/

void __csr_debug__ (csrObject *self, int val)
{
	self->print = val == 0 ? 0 : 1;
	return;
}
