#include "libcsrequest.h"

/* Macros */
#define CSR_MSG "\n\033[1;%dm%s[%d] \033[1;36mlibcsrequest > object.%s\033[0m:\n\t\"%s\"\n\n"

#define CSR_INFO(msg, name) printf(CSR_MSG, 34, "INFO", __LINE__, name, msg)
#define CSR_WARN(msg, name) printf(CSR_MSG, 33, "WARN", __LINE__, name, msg)
#define CSR_ERROR(msg, name) printferr(CSR_MSG, 31, "ERROR", __LINE__, name, msg)

/*---------------------------------------------------------------------------*/

#define CSR_INSERT "INSERT INTO t () VALUES ();"

#define CSR_INSERT_ATT(STR, ITEM) \
	STR = realloc (STR, ((strlen(STR) + strlen(ITEM) + 3) * sizeof(char))); \
	if (!STR) { \
		CSR_WARN("Insufficient space in memory.", "insert(char *table)"); \
		return 0; \
	} \
	strcat(STR, (strlen(STR) > 0 ? ", " : "")); \
	strcat(STR, ITEM);

/*---------------------------------------------------------------------------*/

#define CSR_UPDATE "UPDATE t SET s WHERE ;"

#define CSR_UPDATE_ATT(STR, COL, VAL) \
	STR = realloc (STR, ((strlen(STR) + strlen(COL) + strlen(VAL) + 4) * sizeof(char))); \
	if (!STR) { \
		CSR_WARN("Insufficient space in memory.", "update(char *table)"); \
		return 0; \
	} \
	strcat(STR, (strlen(STR) > 0 ? ", " : "")); \
	strcat(STR, COL); \
	strcat(STR, "="); \
	strcat(STR, VAL);

/*---------------------------------------------------------------------------*/

#define CSR_DELETE "DELETE FROM t WHERE ;;"

/*---------------------------------------------------------------------------*/


static void csr_concat(char *str, ...)
/* concatena diversas strings ao mesmo tempo */
{

}

static void csr_trim(char *str)
/* apara as estremidades da string */
{

}

static int csr_isSelect(char *str)
/* informa se o query é um sql select */
{
	return 0;
}







int __csr_sql__ (csrObject *self, char *query)
{

	printf("query:\n\t%s\n\n", query);

	return 1;
}

/*...........................................................................*/

int __csr_insert__ (csrObject *self, char *table)
{
	/* variáveis locais */
	char *col, *val, *query;
	int result;
	csrData *data = malloc (sizeof(csrData));

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
	data = self->data;

	col = malloc (sizeof(char));
	strcpy(col, "");

	val = malloc (sizeof(char));
	strcpy(val, "");

	/* looping: definindo demais valores */
	while (data != NULL) {
		CSR_INSERT_ATT(col, data->col);
		CSR_INSERT_ATT(val, data->val);
		data = data->next;
	}

	/* construindo query */
	query = malloc ((strlen(CSR_INSERT) + strlen(col) + strlen(val) + 1) * sizeof(char));
	strcpy(query, "INSERT INTO ");
	strcat(query, table);
	strcat(query, " (");
	strcat(query, col);
	strcat(query, ") VALUES (");
	strcat(query, val);
	strcat(query, ");");

	/* executando query */
	result = self->sql(query);

	/* liberando memória */
	free(query);
	free(col);
	free(val);	
	free(data);
	self->clear();

	return result;
}

/*...........................................................................*/

int __csr_update__ (csrObject *self, char *table)
{
	/* variáveis locais */
	char *set, *where, *query;
	int result, whr;
	csrData *data = malloc (sizeof(csrData));

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
	data = self->data;

	set = malloc (sizeof(char));
	strcpy(set, "");

	where = malloc (sizeof(char));
	strcpy(where, "");
	
	whr = 0;

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (data->where == 1 && whr == 0) {
			CSR_UPDATE_ATT(where, data->col, data->val);
			whr = 1;
		} else {
			CSR_UPDATE_ATT(set, data->col, data->val);
		}
		data = data->next;
	}

	/* construindo query */
	query = malloc ((strlen(CSR_UPDATE) + strlen(set) + strlen(where) + 1) * sizeof(char));
	strcpy(query, "UPDATE ");
	strcat(query, table);
	strcat(query, " SET ");
	strcat(query, set);
	if (whr == 1) {
		strcat(query, " WHERE ");
		strcat(query, where);
	}
	strcat(query, ";");

	/* executando query */
	result = self->sql(query);

	/* liberando memória */
	free(query);
	free(set);
	free(where);	
	free(data);
	self->clear();

	return result;
}

/*...........................................................................*/

int __csr_delete__ (csrObject *self, char *table)
{
	/* variáveis locais */
	char *where, *query;
	int result;
	csrData *data = malloc (sizeof(csrData));

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
	data = self->data;

	where = malloc (sizeof(char));
	strcpy(where, "");

	/* looping: definindo demais valores */
	while (data != NULL) {
		if (data->where == 1) {
			CSR_UPDATE_ATT(where, data->col, data->val);
			break;
		}
		data = data->next;
	}

	/* construindo query */
	query = malloc ((strlen(CSR_DELETE) + strlen(where) + 1) * sizeof(char));
	strcpy(query, "DELETE FROM ");
	strcat(query, table);
	if (strlen(where) > 0) {
		strcat(query, " WHERE ");
		strcat(query, where);
	}
	strcat(query, ";");

	/* executando query */
	result = self->sql(query);

	/* liberando memória */
	free(query);
	free(where);	
	free(data);
	self->clear();

	return result;
}

/*...........................................................................*/

int __csr_view__ (csrObject *self, char *table)
{
	return 1;
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

	/* alocando memória */
	data = malloc (sizeof(csrData));
	if (!data) {
		CSR_WARN("Insufficient space in memory.", "add(char *column, char *value)");
		return 0;
	}

	/* data->where */
	data->where = where == 1 ? 1 : 0;

	/* data->col */
	data->col = malloc ((strlen(column) + 1) * sizeof (char));
	if (!data->col) {
		CSR_WARN("Insufficient space in memory for 'column'.", "add(char *column, char *value)");
		return 0;
	}
	strcpy(data->col, column);

	/* data->val */
	data->val = malloc ((value == NULL ? 5 : (strlen(column) + 3)) * sizeof (char));
	if (!data->val) {
		CSR_WARN("Insufficient space in memory for 'value'.", "add(char *column, char *value)");
		return 0;
	}
	if (value == NULL) {
		strcpy(data->val, "NULL");
	} else {
		strcpy(data->val, "'");
		strcat(data->val, value);
		strcat(data->val, "'");
	}

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
		printf("col: %s\nval: %s\nwhere: %d\n\n", data->col, data->val, data->where);
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
