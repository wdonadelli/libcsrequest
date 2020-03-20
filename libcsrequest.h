/*-----------------------------------------------------------------------------
Library CSrequest (v1.0.0)

This is a library written in C language designed to simplify requests to the
SQLite database.
(see https://www.sqlite.org/index.html)

MIT License

Copyright (c) 2020 Willian Donadelli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Ubuntu package
	libsqlite3-dev

The library is hosted at https://github.com/wdonadelli/libcsrequest and the manual
at https://wdonadelli.github.io/libcsrequest/.

Willian Donadelli <wdonadelli@gmail.com>
-----------------------------------------------------------------------------*/

#ifndef LIBRARY_CS_REQUEST_H

	#define LIBRARY_CS_REQUEST_H

/*-----------------------------------------------------------------------------
	A presente biblioteca exige as seguintes bibliotecas
-----------------------------------------------------------------------------*/
	#include <sqlite3.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <string.h>

/*-----------------------------------------------------------------------------
	Estrutura dos campos do SQL
	https://www.sqlite.org/limits.html
		SQLITE_MAX_LENGTH 1000000000
		SQLITE_MAX_COLUMN 2000
		SQLITE_MAX_SQL_LENGTH 1000000
		18446744073709551616
-----------------------------------------------------------------------------*/
	typedef struct csrData
	{
		char *col;
		char *val;
		unsigned int where: 1;
		struct csrData *next;
	} csrData;

/*-----------------------------------------------------------------------------
	Estrutura do objeto
-----------------------------------------------------------------------------*/
	typedef struct
	{
		/*-- Métodos --*/
		int (*sql)();    /* executa uma instrução SQL */
		int (*insert)(); /* constrói uma instrução INSERT (ver método add) */
		int (*update)(); /* constrói uma instrução UPDATE (ver método add) */
		int (*delete)(); /* constrói uma instrução DELETE (ver método add) */
		int (*select)(); /* constrói uma instrução SELECT (ver método add) */
		int (*add)();    /* registra informações para contrutores acima */
		int (*clear)();  /* apaga registros (ver método add) */
		int (*query)();  /* retorna o valor da coluna (ver método sql) */
		
		/*-- Atributos --*/
		char *file;            /* caminho para o banco de dados */
		unsigned int error: 1; /* registra erro na última operação */
		char *msg;             /* registra mensagem de erro da última operação */
		csrData *data;         /* lista de registros */
		char **col;            /* guarda o array com nomes das colunas */
		char **val;            /* guarda o array com valores das colunas */
		int len;               /* guarda a quantidade de colunas */
		int row;               /* guarda a linha da pesquisa */
	} csrObject;
	
/*-----------------------------------------------------------------------------
	Protótipos

	Os protótipos abaixo não devem ser utilizadas para manipulação da
	biblioteca, sua utilidade se destina a criação do métodos secundários da
	ferramenta principal.
	Todos os protótipos retornam 1 (sucesso) ou 0 (erro)
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	__csr_sql__ () executa uma instrução SQL
	query:  string contendo a instrução SQL
	reader: função a executar durante a pesquisa (opcional)
	Retorna 1 no caso de sucesso e 0 no caso de insucesso
-----------------------------------------------------------------------------*/
	int __csr_sql__ (csrObject *self, char *query, void (*reader)());

	#define __CSR_SQL__(SELF)												\
		int __csr_sql__##SELF (char *query, void (*reader)())		\
		{																			\
			return __csr_sql__(&SELF, query, reader);					\
		}																			\
		SELF.sql = __csr_sql__##SELF;										\

/*-----------------------------------------------------------------------------
	__csr_insert__ () constrói uma instrução INSERT e a executa a partir das
	informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
	Retorna 1 no caso de sucesso e 0 no caso de insucesso
-----------------------------------------------------------------------------*/
	int __csr_insert__ (csrObject *self, char *table);

	#define __CSR_INSERT__(SELF)						\
		int __csr_insert__##SELF (char *table)		\
		{														\
			return __csr_insert__(&SELF, table);	\
		}														\
		SELF.insert = __csr_insert__##SELF;			\

/*-----------------------------------------------------------------------------
	__csr_update__ () constrói uma instrução UPDATE e a executa a partir das
	informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
	Retorna 1 no caso de sucesso e 0 no caso de insucesso
-----------------------------------------------------------------------------*/
	int __csr_update__ (csrObject *self, char *table);

	#define __CSR_UPDATE__(SELF)						\
		int __csr_update__##SELF (char *table)		\
		{														\
			return __csr_update__(&SELF, table);	\
		}														\
		SELF.update = __csr_update__##SELF;			\

/*-----------------------------------------------------------------------------
	__csr_delete__ () constrói uma instrução DELETE e a executa a partir das
	informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
	Retorna 1 no caso de sucesso e 0 no caso de insucesso
-----------------------------------------------------------------------------*/
	int __csr_delete__ (csrObject *self, char *table);

	#define __CSR_DELETE__(SELF)						\
		int __csr_delete__##SELF (char *table)		\
		{														\
			return __csr_delete__(&SELF, table);	\
		}														\
		SELF.delete = __csr_delete__##SELF;			\

/*-----------------------------------------------------------------------------
	__csr_select__ () constrói uma instrução SELECT e a executa a partir das
	informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
	reader: função a executar durante a pesquisa (opcional)
	Retorna 1 no caso de sucesso e 0 no caso de insucesso
-----------------------------------------------------------------------------*/
	int __csr_select__ (csrObject *self, char *table, void (*reader)());

	#define __CSR_SELECT__(SELF)												\
		int __csr_select__##SELF (char *table, void (*reader)())		\
		{																				\
			return __csr_select__(&SELF, table, reader);					\
		}																				\
		SELF.select = __csr_select__##SELF;									\

/*-----------------------------------------------------------------------------
	__csr_add__ () adiciona registros para execução dos métodos construtores de
	instruções SQL
	column: string contendo o nome da coluna
	value:  string contendo o valor da coluna
	where:  se 1, informado como critério de pesquisa (update/delete/select)
	Retorna 1 no caso de sucesso e 0 no caso de insucesso
-----------------------------------------------------------------------------*/
	int __csr_add__ (csrObject *self, char *column, char *value, int where);

	#define __CSR_ADD__(SELF)														\
		int __csr_add__##SELF (char *column, char *value, int where)	\
		{																					\
			return __csr_add__(&SELF, column, value, where);				\
		}																					\
		SELF.add = __csr_add__##SELF;												\

/*-----------------------------------------------------------------------------
	__csr_clear__ () limpa os dados adicionados pelo método add
	Retorna 1 no caso de sucesso e 0 no caso de insucesso
-----------------------------------------------------------------------------*/
	int __csr_clear__ (csrObject *self);

	#define __CSR_CLEAR__(SELF)				\
		int __csr_clear__##SELF ()				\
		{												\
			return __csr_clear__(&SELF);		\
		}												\
		SELF.clear = __csr_clear__##SELF;	\

/*-----------------------------------------------------------------------------
	__csr_query__ () retorna o valor da coluna a partir de seu nome ou sequência
	value:  ponteiro para string que receberá o valor da coluna
	column: string contendo o nome da coluna
	number: inteiro informando sequência da coluna (column precisa NULL)
	Retorna um ponteiro (string) para o valor da coluna
-----------------------------------------------------------------------------*/
	int __csr_query__ (csrObject *self, char *value, char *column, int item);

	#define __CSR_QUERY__(SELF)													\
		int __csr_query__##SELF (char *value, char *column, int item)	\
		{																					\
			return __csr_query__(&SELF, value, column, item);				\
		}																					\
		SELF.query = __csr_query__##SELF;										\



/*-----------------------------------------------------------------------------
	new_CSrequest () construtor da estrutura
-----------------------------------------------------------------------------*/
	#define new_CSrequest(OBJECT, FILE)										\
																						\
		csrObject OBJECT;															\
		OBJECT.file = malloc ((strlen(FILE) + 1) * sizeof (char));	\
		strcpy(OBJECT.file, FILE);												\
		OBJECT.error = 0;															\
		OBJECT.msg = malloc (2 * sizeof (char));							\
		strcpy(OBJECT.msg, "");													\
		OBJECT.data = NULL;														\
		OBJECT.col = NULL;														\
		OBJECT.val = NULL;														\
		OBJECT.len = 0;															\
		OBJECT.row = 0;															\
		__CSR_SQL__(OBJECT);														\
		__CSR_INSERT__(OBJECT);													\
		__CSR_UPDATE__(OBJECT);													\
		__CSR_DELETE__(OBJECT);													\
		__CSR_SELECT__(OBJECT);													\
		__CSR_ADD__(OBJECT);														\
		__CSR_CLEAR__(OBJECT);													\
		__CSR_QUERY__(OBJECT);													\


#endif
