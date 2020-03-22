/*-----------------------------------------------------------------------------
Library CSRequest (v1.0.0) <wdonadelli@gmail.com>

This is a library written in C language designed to simplify requests to the
SQLite database.

https://www.sqlite.org/index.html
https://github.com/wdonadelli/libcsrequest
https://wdonadelli.github.io/libcsrequest/

Ubuntu 18 LTS package: libsqlite3-dev

GCC compilation: gcc -c libcsrequest.c -l sqlite3

-------------------------------------------------------------------------------
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
-----------------------------------------------------------------------------*/

#ifndef LIBRARY_CSR_H

	#define LIBRARY_CSR_H

/*-----------------------------------------------------------------------------
	A presente biblioteca exige as seguintes bibliotecas
-----------------------------------------------------------------------------*/
	#include <sqlite3.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <string.h>

/*-----------------------------------------------------------------------------
	Estrutura dos registros
	https://www.sqlite.org/limits.html
		SQLITE_MAX_LENGTH 1000000000
		SQLITE_MAX_COLUMN 2000
		SQLITE_MAX_SQL_LENGTH 1000000
		18446744073709551616
-----------------------------------------------------------------------------*/
	typedef struct csrData
	{
		char *col;             /* nome da coluna */
		char *val;             /* valor da coluna */
		unsigned int where: 1; /* informação sobre o filtro WHERE */
		struct csrData *next;  /* próximo registro */
	} csrData;

/*-----------------------------------------------------------------------------
	Estrutura do objeto
-----------------------------------------------------------------------------*/
	typedef struct
	{
		/*-- Métodos --*/
		int (*sql)();		/* executa uma instrução SQL */
		int (*insert)();	/* constrói uma instrução INSERT (ver método add) */
		int (*update)();	/* constrói uma instrução UPDATE (ver método add) */
		int (*delete)();	/* constrói uma instrução DELETE (ver método add) */
		int (*select)();	/* constrói uma instrução SELECT (ver método add) */
		int (*add)();		/* registra informações para contrutores acima */
		int (*clear)();	/* apaga registros (ver método add) */
		char *(*fetch)();	/* retorna o valor da coluna (ver método sql) */
		void (*reader)();	/* guarda a função de pesquisa */
		
		/*-- Atributos --*/
		char *file;					/* caminho para o banco de dados */
		unsigned int error: 1;	/* registra erro na última operação */
		char *msg;					/* registra mensagem de erro da última operação */
		unsigned long int row;	/* guarda o número da linha */
		unsigned int len;			/* guarda o número de colunas */
		char **col;					/* guarda o array com nomes das colunas */
		char **val;					/* guarda o array com valores das colunas */
		csrData *data;				/* guarda a estrutura de registros */
		
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
	__csr_fetch__ () retorna a string com o valor da coluna na instrução SELECT
	Retorna NULL se a coluna é nula ou não foi encontrada
-----------------------------------------------------------------------------*/
	char *__csr_fetch__ (csrObject *self, char *col);

	#define __CSR_FETCH__(SELF)					\
		char *__csr_fetch__##SELF (char *col)	\
		{													\
			return __csr_fetch__(&SELF, col);	\
		}													\
		SELF.fetch = __csr_fetch__##SELF;		\

/*-----------------------------------------------------------------------------
	new_csr () construtor da estrutura
-----------------------------------------------------------------------------*/
	#define new_csr(OBJECT, FILE)												\
																						\
		csrObject OBJECT;															\
		OBJECT.file = malloc ((strlen(FILE) + 1) * sizeof (char));	\
		OBJECT.msg  = malloc (2 * sizeof (char));							\
		strcpy(OBJECT.file, FILE);												\
		strcpy(OBJECT.msg, "");													\
		OBJECT.error  = 0;														\
		OBJECT.row    = 0;														\
		OBJECT.len    = 0;														\
		OBJECT.data   = NULL;													\
		OBJECT.col    = NULL;													\
		OBJECT.val    = NULL;													\
		OBJECT.reader = NULL;													\
																						\
		__CSR_SQL__(OBJECT);														\
		__CSR_INSERT__(OBJECT);													\
		__CSR_UPDATE__(OBJECT);													\
		__CSR_DELETE__(OBJECT);													\
		__CSR_SELECT__(OBJECT);													\
		__CSR_ADD__(OBJECT);														\
		__CSR_CLEAR__(OBJECT);													\
		__CSR_FETCH__(OBJECT);													\


#endif
