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
#define CSR_NO_CALLBACK    "The 'reader' function is required for queries."
#define CSR_INVALID_COLUMN "Invalid 'column' argument."
#define CSR_SINGLE_QUOTES  "Single quotes must be used in double in SQLite."
#define CSR_FETCH_NO_QUERY "Use the method only in queries (SELECT statements)"
#define CSR_EMPTY_QUERY    "Empty SQL statement."

/* macro dos protótipos */
#define CSR_SQL     "sql(char *query, void (*reader)())"
#define CSR_SELECT  "select(char *table, void (*reader)())"
#define CSR_INSERT  "insert(char *table)"
#define CSR_REPLACE "replace(char *table)"
#define CSR_UPDATE  "update(char *table)"
#define CSR_DELETE  "delete(char *table)"
#define CSR_CREATE  "create(char *table)"
#define CSR_DROP    "drop(char *table)"
#define CSR_ADD     "add(char *column, char *value)"
#define CSR_CLEAR   "clear()"
#define CSR_FETCH   "fetch(char *column)"

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

/* guarda informação se a função sql foi chamada por uma função de atalho */
static int csr_shortcut = 0;

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

static char *csr_constraint(char *val)
/* retorna o valor de add sem aspas ou NULL se string vazia */
{
	/* variáveis iniciais */
	char *cnst = NULL;

	/* removendo aspas simples das pontas */
	if (strcmp(val, "NULL") != 0) {
		cnst = malloc(strlen(val) * sizeof(char));
		CSR_CHECK_MEMORY(cnst);
		strcpy(cnst, ++val);
		cnst[strlen(cnst)-1] = '\0';
	}

	/* retorna nul se a string é vazia */
	return (csr_is_empty(cnst) ? NULL : cnst);
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

static void csr_set_status (csrObject *self, int code, char *msg)
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
	if (self->message != NULL) {free(self->message);}
	self->message = msg == NULL ? NULL : csr_cat(msg, NULL);
	return;
}

/*...........................................................................*/

static void csr_clear_status (csrObject *self)
/* reinicia o status */
{
	csr_set_status(self, CSR_OK, NULL);
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
		csr_set_status(self, CSR_ERR, CSR_NO_CALLBACK);
		return self->status();
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
	/* Linha para imprimir o argumento query (somente para testes) */
	//printf("---\n%s\n---\n\n", query);//FIXME tenho que comentar essa linha

	/* definindo prototype e limpando status */
	if (csr_shortcut) {
		csr_shortcut = 0;
	} else {
		csr_prototype = CSR_SQL;
	}
	csr_clear_status(self);

	/* verificando se query foi informado */
	if (csr_is_empty(query)) {
		csr_set_status(self, CSR_ERR, CSR_EMPTY_QUERY);
		return self->status();
	}

	/* variáveis locais */
	sqlite3 *db;
	int open, exec;
	char *error = NULL;
	
	/* valores iniciais */
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
		csr_set_status(self, CSR_FAIL, (char *) sqlite3_errmsg(db));
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
	if (exec == SQLITE_OK) {

		/* fechar banco em caso de sucesso */
		if (sqlite3_close(db) != SQLITE_OK) {
			csr_set_status(self, CSR_FAIL, error);
		}

		/* zerar registros no caso de função de atalho sem erro */
		if (strcmp(csr_prototype, CSR_SQL) != 0) {
			self->clear();
		}

	} else if (self->status() == CSR_ERR) {
		/* fechar banco em caso de erro (sem verificar sucesso) */
		sqlite3_close(db);
	} else {
		csr_set_status(self, CSR_FAIL, error);
		/*fechar banco em caso de falha (sem verificar sucesso) */
		sqlite3_close(db);
	}

	/* liberando memória e retornando */
	sqlite3_free(error);

	return self->status();
}

/*...........................................................................*/

int __csr_select__ (csrObject *self, char *table, void (*reader)())
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_SELECT;
	csr_clear_status(self);

	/* variáveis locais */
	csrData *data = self->data;
	char *query   = NULL;
	char *col     = NULL;
	char *where   = NULL;
	char *trash1  = NULL;
	int result;

	/* testando informações */
	if (!csr_is_name(table)) {
		csr_set_status(self, CSR_ERR, CSR_INVALID_TABLE);
	} else if (reader == NULL) {
		csr_set_status(self, CSR_ERR, CSR_NO_CALLBACK);
	}
	if (self->status() != CSR_OK) {
		return self->status();
	}

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (!data->where) {
			/* o que não for WHERE, registrar */
			col = csr_cat(
				(csr_is_empty(col) ? "" : col),
				(csr_is_empty(col) ? "" : ", "),
				data->col,
				NULL
			);

			/* liberando memória e guardando ponteiro */
			if (trash1 != NULL) {free(trash1);}
			trash1 = col;

		} else if (csr_is_empty(where)) {
			/* se for WHERE e ainda não estiver definido, registrar */
			where = csr_set_where(data->col, data->val);
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
	csr_shortcut = 1;
	result = self->sql(query, reader);

	/* liberando memória e retornando */
	if (query != NULL) {free(query);}
	if (col   != NULL) {free(col);}
	if (where != NULL) {free(where);}
	return result;
}

/*...........................................................................*/

int __csr_insert__ (csrObject *self, char *table)
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_INSERT;
	csr_clear_status(self);

	/* variáveis locais */
	csrData *data = self->data;
	char *query   = NULL;
	char *col     = NULL;
	char *val     = NULL;
	char *trash1  = NULL;
	char *trash2  = NULL;
	int result;

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (!data->where) {
			/* ignora os registro com where ligado */
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

			/* liberando memória e guardando ponteiro provisório */
			if (trash1  != NULL) {free(trash1);}
			if (trash2  != NULL) {free(trash2);}
			trash1 = col;
			trash2 = val;
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
	}

	/* em caso de erro, liberar memória e retornar */
	if (self->status() != CSR_OK) {
		if (query != NULL) {free(query);}
		if (col   != NULL) {free(col);}
		if (val   != NULL) {free(val);}
		return self->status();
	}

	/* construindo query */
	query = csr_cat(
		"INSERT INTO ", table, " (", col, ") VALUES (", val, ");", NULL
	);

	/* executando query e obterndo o retorno */
	csr_shortcut = 1;
	result = self->sql(query, NULL);

	/* liberando memória e retornando */
	if (query != NULL) {free(query);}
	if (col   != NULL) {free(col);}
	if (val   != NULL) {free(val);}
	return result;
}

/*...........................................................................*/

int __csr_replace__ (csrObject *self, char *table)
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_REPLACE;
	csr_clear_status(self);

	/* variáveis locais */
	csrData *data = self->data;
	char *query   = NULL;
	char *col     = NULL;
	char *val     = NULL;
	char *trash1  = NULL;
	char *trash2  = NULL;
	int result;

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (!data->where) {
			/* ignora os registro com where ligado */
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

			/* liberando memória e guardando ponteiro provisório */
			if (trash1 != NULL) {free(trash1);}
			if (trash2 != NULL) {free(trash2);}
			trash1 = col;
			trash2 = val;
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
	}

	/* em caso de erro, liberar memória e retornar */
	if (self->status() != CSR_OK) {
		if (query != NULL) {free(query);}
		if (col   != NULL) {free(col);}
		if (val   != NULL) {free(val);}
		return self->status();
	}

	/* construindo query */
	query = csr_cat(
		"REPLACE INTO ", table, " (", col, ") VALUES (", val, ");", NULL
	);

	/* executando query */
	csr_shortcut = 1;
	result = self->sql(query, NULL);

	/* liberando memória e retornando */
	if (query != NULL) {free(query);}
	if (col   != NULL) {free(col);}
	if (val   != NULL) {free(val);}
	return result;
}

/*...........................................................................*/

int __csr_update__ (csrObject *self, char *table)
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_UPDATE;
	csr_clear_status(self);

	/* variáveis locais */
	csrData *data = self->data;
	char *query   = NULL;
	char *set     = NULL;
	char *where   = NULL;
	char *trash1  = NULL;
	int result;

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (!data->where) {
			/* se não for WHERE, pegar o valor */
			set = csr_cat(
				(csr_is_empty(set) ? "" : set),
				(csr_is_empty(set) ? "" : ", "),
				data->col,
				" = ",
				data->val,
				NULL
			);

			/* liberando memória e guardando ponteiro */
			if (trash1 != NULL) {free(trash1);}
			trash1 = set;

		} else if (csr_is_empty(where)) {
			/* se for WHERE e ainda não estiver definido, pegar o valor */
			where = csr_set_where(data->col, data->val);
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
	}

	/* em caso de erro, liberar memória e retornar */
	if (self->status() != CSR_OK) {
		if (query != NULL) {free(query);}
		if (set   != NULL) {free(set);}
		if (where != NULL) {free(where);}
		return self->status();
	}

	/* construindo query */
	query = csr_cat(
		"UPDATE ", table, " SET ", set, where, ";", NULL
	);

	/* executando query */
	csr_shortcut = 1;
	result = self->sql(query, NULL);

	/* liberando memória e retornando */
	if (query != NULL) {free(query);}
	if (set   != NULL) {free(set);}
	if (where != NULL) {free(where);}
	return result;
}

/*...........................................................................*/

int __csr_delete__ (csrObject *self, char *table)
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_DELETE;
	csr_clear_status(self);

	/* variáveis locais */
	csrData *data = self->data;
	char *query   = NULL;
	char *where   = NULL;
	int result;

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (!csr_is_empty(where)) {
			/* se o WHERE já tiver definido, sai do looping */
			break;
		} else if (data->where) {
			/* se for WHERE, pega o valor */
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
	}

	/* em caso de erro, liberar memória e retornar */
	if (self->status() != CSR_OK) {
		if (query != NULL) {free(query);}
		if (where != NULL) {free(where);}
		return self->status();
	}

	/* construindo query */
	query = csr_cat(
		"DELETE FROM ", table, where, ";", NULL
	);

	/* executando query */
	csr_shortcut = 1;
	result = self->sql(query, NULL);

	/* liberando memória e retornando */
	if (query != NULL) {free(query);}
	if (where != NULL) {free(where);}
	return result;
}

/*...........................................................................*/

int __csr_create__ (csrObject *self, char *table)
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_CREATE;
	csr_clear_status(self);

	/* variáveis locais */
	csrData *data = self->data;
	char *query   = NULL;
	char *col     = NULL;
	char *trash1  = NULL;
	char *cnstrnt = NULL;
	int result;

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (!data->where) {
			/* ignora os registro com where ligado */
			cnstrnt = csr_constraint(data->val);
			col = csr_cat(
				csr_is_empty(col) ? "" : col,
				csr_is_empty(col) ? "" : ", ",
				data->col,
				cnstrnt == NULL ? "" : " ",
				cnstrnt == NULL ? "" : cnstrnt,
				NULL
			);
			/* liberando memória */
			free(cnstrnt);

			/* liberando memória e guardando ponteiro provisório */
			if (trash1 != NULL) {free(trash1);}
			trash1 = col;
		}
		data = data->next;
	}

	/* verificando erros */
	if (self->data == NULL) {
		csr_set_status(self, CSR_ERR, CSR_SELF_DATA_NULL);
	} else if (!csr_is_name(table)) {
		csr_set_status(self, CSR_ERR, CSR_INVALID_TABLE);
	} else if (csr_is_empty(col)) {
		csr_set_status(self, CSR_ERR, CSR_BROKEN_SQL);
	}

	/* em caso de erro, liberar memória e retornar */
	if (self->status() != CSR_OK) {
		if (query != NULL) {free(query);}
		if (col   != NULL) {free(col);}
		return self->status();
	}

	/* construindo query */
	query = csr_cat("CREATE TABLE ", table, " (", col, ");", NULL);

	/* executando query */
	csr_shortcut = 1;
	result = self->sql(query, NULL);

	/* liberando memória e retornando */
	if (query != NULL) {free(query);}
	if (col   != NULL) {free(col);}
	return result;
}

/*...........................................................................*/

int __csr_drop__ (csrObject *self, char *table)
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_DROP;
	csr_clear_status(self);

	/* variáveis locais */
	char *query = NULL;
	int result;

	/* verificando erros */
	if (!csr_is_name(table)) {
		csr_set_status(self, CSR_ERR, CSR_INVALID_TABLE);
		return self->status();
	}

	/* construindo query */
	query = csr_cat("DROP TABLE ", table, ";", NULL);

	/* executando query */
	csr_shortcut = 1;
	result = self->sql(query, NULL);

	/* liberando memória e retornando */
	if (query != NULL) {free(query);}
	return result;
}

/*...........................................................................*/

int __csr_add__ (csrObject *self, char *column, char *value, int where)
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_ADD;
	csr_clear_status(self);

	/* definindo variáveis locais */
	csrData *data;

	/* verificar se a coluna foi informada */
	if (!csr_is_name(column)) {
		csr_set_status(self, CSR_ERR, CSR_INVALID_COLUMN);
		return self->status();
	}

	/* informar sobre a existência de apóstrofo */
	if (value != NULL && strstr(value, "'") != NULL) {
		CSR_INFO(CSR_SINGLE_QUOTES);
	}

	/* definindo variáveis */
	where = where == 1 ? 1 : 0;

	/* verificando se já existe coluna informada e redefinir valores */
	data = self->data;
	while(data) {
		if (strcmp(data->col, column) == 0 && data->where == where) {
			/* liberando memória do valor anterior */
			free(data->val);
			
			/* definindo novo valor */
			if (value == NULL) {
				data->val = csr_cat("NULL", NULL);
			} else {
				data->val = csr_cat("'", value, "'", NULL);
			}
			break;
		}
		data = data->next;
	}

	/* se	não encontrou coluna, definir nova */
	if (data == NULL) {
		data = malloc(sizeof(csrData));
		CSR_CHECK_MEMORY(data);
		data->where = where;
		data->col = csr_cat(column, NULL);
		if (value == NULL) {
			data->val = csr_cat("NULL", NULL);
		} else {
			data->val = csr_cat("'", value, "'", NULL);
		}
		data->next  = self->data;
		self->data  = data;
	}

	return self->status();
}

/*...........................................................................*/

int __csr_clear__ (csrObject *self)
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_CLEAR;
	csr_clear_status(self);

	/* definindo variáveis locais */
	csrData *data, *old;
	data = self->data;

	/* looping para liberar as memórias alocadas */
	while (data != NULL) {
		data->where = 0;
		old  = data;
		data = data->next;
		free(old->col);
		free(old->val);
		free(old);
	}

	/* zerando self->data */
	self->data = NULL;

	return self->status();
}

/*...........................................................................*/

char *__csr_fetch__ (csrObject *self, char *column)
{
	/* definindo prototype e limpando status */
	csr_prototype = CSR_FETCH;
	csr_clear_status(self);

	/* variáveis locais */
	int i;
	char *info;

	/* verificar se a coluna foi informada */
	if (!csr_is_name(column)) {
		csr_set_status(self, CSR_ERR, CSR_INVALID_COLUMN);
		return NULL;
	}

	/* verificando e redefinindo valores se necessário */
	if (self->col == NULL || self->val == NULL) {
		self->col    = NULL;
		self->val    = NULL;
		self->row    = 0;
		self->len    = 0;
		self->reader = NULL;
		csr_set_status(self, CSR_ERR, CSR_FETCH_NO_QUERY);
		return NULL;
	}

	/* looping para encontrar coluna */
	for(i = 0; i < self->len; i++){
		if (strcmp(self->col[i], column) == 0) {
			return self->val[i];
		}
	}

	/* se nada for encontrado */
	info = csr_cat("'", column, "' column not found.", NULL);
	csr_set_status(self, CSR_ERR, info);
	free(info);
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

/*...........................................................................*/

void __csr_free__ (csrObject *self)
{
	csr_clear_status(self);
	self->clear();
}
