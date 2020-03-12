#include "libcsrequest.h"

/* Macros */
#define CSR_MSG "\n\033[1;%dm%s[%d] \033[1;36mlibcsrequest > object.%s\033[0m:\n\t\"%s\"\n\n"

#define CSR_INFO(msg, name) printf(CSR_MSG, 34, "INFO", __LINE__, name, msg)
#define CSR_WARN(msg, name) printf(CSR_MSG, 33, "WARN", __LINE__, name, msg)
#define CSR_ERROR(msg, name) printferr(CSR_MSG, 31, "ERROR", __LINE__, name, msg)

#define CSR_INSERT(TAB, COL, VAL) "INSERT INTO TAB ( COL ) VALUES ( VAL );"

#define CSR_INSERT_ATT(STR, ITEM) \
	STR = realloc (STR, ((strlen(STR) + strlen(ITEM) + 3) * sizeof(char))); \
	if (!STR) { \
		CSR_WARN("Insufficient space in memory.", "add(char *column, char *value)"); \
		return 0; \
	} \
	strcat(STR, ", "); \
	strcat(STR, ITEM);


int __csr_sql__ (csrObject *self, char *query)
{
	return 1;
}

/*...........................................................................*/

int __csr_insert__ (csrObject *self, char *table)
{
	/* variáveis locais */
	char *col, *val;
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
	col = malloc ((strlen(data->col) + 1) * sizeof(char));
	strcpy(col, data->col);
	val = malloc ((strlen(data->val) + 1) * sizeof(char));
	strcpy(val, data->val);

	/* looping: definindo demais valores */
	data = data->next;

	while (data != NULL) {
		CSR_INSERT_ATT(col, data->col);
		CSR_INSERT_ATT(val, data->val);
		data = data->next;
	}

	char tosco[] = CSR_INSERT(table, col, val);
	
	printf("%s\n\n", tosco);

	return 1;
}

/*...........................................................................*/

int __csr_update__ (csrObject *self, char *table)
{
	return 1;
}

/*...........................................................................*/

int __csr_delete__ (csrObject *self, char *table)
{
	return 1;
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
