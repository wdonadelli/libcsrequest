#include "libcsrequest.h"

/* Macros */
#define CSR_MSG "\n\033[1;%dm%s[%d] \033[1;36mlibcsrequest > object.%s\033[0m:\n\t\"%s\"\n\n"

#define CSR_INFO(msg, name) printf(CSR_MSG, 34, "INFO", __LINE__, name, msg)
#define CSR_WARN(msg, name) printf(CSR_MSG, 33, "WARN", __LINE__, name, msg)
#define CSR_ERROR(msg, name) printferr(CSR_MSG, 31, "ERROR", __LINE__, name, msg);



int __csr_sql__ (csrObject *self, char *query)
{
	return 1;
}

/*...........................................................................*/

int __csr_insert__ (csrObject *self)
{
	return 1;
}

/*...........................................................................*/

int __csr_update__ (csrObject *self, char *column, char *value)
{
	return 1;
}

/*...........................................................................*/

int __csr_delete__ (csrObject *self, char *column, char *value)
{
	return 1;
}

/*...........................................................................*/

int __csr_view__ (csrObject *self, char *column, char *value)
{
	return 1;
}

/*...........................................................................*/

int __csr_add__ (csrObject *self, char *column, char *value)
{
	/* definindo variáveis locais */
	csrData data;
	int count;

	/* verificar se a coluna foi informada */
	if (column == NULL) {
		CSR_WARN("The \"column\" argument is mandatory.", "add(char *column, char *value)");
		return 0;
	}

	/* definindo coluna */
	data.col = malloc ((strlen(column) + 1) * sizeof (char));

	if (!data.col) {
		CSR_WARN("Insufficient space in memory for \"column\".", "add(char *column, char *value)");
		return 0;
	}

	strcpy(data.col, column);
	
	/* definindo valor */
	data.val = value == NULL ? malloc (5 * sizeof(char)) : malloc ((strlen(value) + 3) * sizeof(char));

	if (!data.val) {
		CSR_WARN("Insufficient space in memory for \"value\".", "add(char *column, char *value)");
		return 0;
	}
	
	if (value == NULL) {
		strcpy(data.val, "NULL");
	} else {
		strcpy(data.val, "'");
		strcat(data.val, value);
		strcat(data.val, "'");
	}

	/* definindo o tamanho do array self->data */
	printf("csrData: %li\n", sizeof(csrData));
	printf("self->data 1: %li\n", sizeof(self->data[0]));

	count = sizeof(self->data) / sizeof(csrData);
	printf("count: %i\n", count);


	self->data[count] = data;

//	self->data = count == 0 ? malloc (sizeof(data)) : malloc (sizeof(self->data) + sizeof(data));
//	self->data[0] = data;

//	if (!self->data) {
//		CSR_WARN("Insufficient space in memory to add more data.", "add(char *column, char *value)");
//		return 0;
//	}

	printf("self->data 2: %li\n", sizeof(self->data));
	
	printf("self->data[x].val: %s\n", self->data[count].val);

/*
	while (self->data[i] != NULL) {
		printf("idiota: %i\n", i);
		if (i == 10) break;
		i = i + 1;
	}
*/
	
	/* adicionando ao array */


	printf("---------------------\n");



	return 1;
}

/*...........................................................................*/

int __csr_clear__ (csrObject *self)
{
	return 1;
}
