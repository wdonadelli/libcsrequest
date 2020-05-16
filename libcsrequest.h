/*-----------------------------------------------------------------------------
Library CSRequest (v1.1.0) <wdonadelli@gmail.com>

This is a library written in C language designed to simplify requests to the
SQLite database.

https://www.sqlite.org/
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
		/*-- Métodos acessíveis ao usuário --*/
		int (*sql)();     /* executa uma instrução SQL */
		int (*insert)();  /* constrói uma instrução INSERT (ver método add) */
		int (*replace)(); /* constrói uma instrução REPLACE (ver método add) */
		int (*update)();  /* constrói uma instrução UPDATE (ver método add) */
		int (*delete)();  /* constrói uma instrução DELETE (ver método add) */
		int (*select)();  /* constrói uma instrução SELECT (ver método add) */
		int (*create)();  /* constrói uma instrução CREATE TABLE (ver método add) */
		int (*drop)();    /* constrói uma instrução DROP TABLE */
		int (*add)();     /* registra informações para contrutores acima */
		int (*clear)();   /* apaga registros (ver método add) */
		char *(*fetch)(); /* retorna o valor da coluna (ver método sql) */
		int (*status)();  /* retorna o status da última solicitação */
		char *(*info)();  /* retorna a mensagem da última solicitação */
		void (*debug)();  /* liga/desliga a depuração */
		
		/*-- Métodos/Atributos inacessíveis ao usuário --*/
		void (*reader)();      /* guarda a função de pesquisa */
		char *file;            /* caminho para o banco de dados */
		int print;             /* registra o acionamento da depuração */
		int code;              /* registra o retorno da última operação */
		char *message;         /* registra mensagem de erro da última operação */
		unsigned long int row; /* guarda o número da linha */
		unsigned int len;      /* guarda o número de colunas */
		char **col;            /* guarda o array com nomes das colunas */
		char **val;            /* guarda o array com valores das colunas */
		csrData *data;         /* guarda a estrutura de registros */
		
	} csrObject;

/*-----------------------------------------------------------------------------
	Protótipos

	Os protótipos abaixo não devem ser utilizadas para manipulação da
	biblioteca, sua utilidade se destina a criação do métodos secundários da
	ferramenta principal.
	Retornos dos protótipos:
		CSR_OK    se não ocorreu erro na requisição SQL ou nos métodos
		CSR_FAIL  se ocorreu erro na requisição SQL
		CSR_ERROR se ocorreu erro nos parâmetros do método 
-----------------------------------------------------------------------------*/
	#define CSR_OK   0
	#define CSR_FAIL 1
	#define CSR_ERR -1

/*-----------------------------------------------------------------------------
	__csr_sql__ () executa uma instrução SQL
	query:  string contendo a instrução SQL
	reader: função a executar durante a pesquisa (opcional)
-----------------------------------------------------------------------------*/
	int __csr_sql__ (csrObject *self, char *query, void (*reader)());

	#define __CSR_SQL__(SELF)                                \
		int __csr_sql__##SELF (char *query, void (*reader)()) \
		{                                                     \
			return __csr_sql__(&SELF, query, reader);          \
		}                                                     \
		SELF.sql = __csr_sql__##SELF;                         \

/*-----------------------------------------------------------------------------
	__csr_insert__ () constrói uma instrução INSERT e a executa a partir das
	informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
-----------------------------------------------------------------------------*/
	int __csr_insert__ (csrObject *self, char *table);

	#define __CSR_INSERT__(SELF)               \
		int __csr_insert__##SELF (char *table)  \
		{                                       \
			return __csr_insert__(&SELF, table); \
		}                                       \
		SELF.insert = __csr_insert__##SELF;     \

/*-----------------------------------------------------------------------------
	__csr_replace__ () constrói uma instrução REPLACE e a executa a partir das
	informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
-----------------------------------------------------------------------------*/
	int __csr_replace__ (csrObject *self, char *table);

	#define __CSR_REPLACE__(SELF)               \
		int __csr_replace__##SELF (char *table)  \
		{                                        \
			return __csr_replace__(&SELF, table); \
		}                                        \
		SELF.replace = __csr_replace__##SELF;    \

/*-----------------------------------------------------------------------------
	__csr_update__ () constrói uma instrução UPDATE e a executa a partir das
	informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
-----------------------------------------------------------------------------*/
	int __csr_update__ (csrObject *self, char *table);

	#define __CSR_UPDATE__(SELF)               \
		int __csr_update__##SELF (char *table)  \
		{                                       \
			return __csr_update__(&SELF, table); \
		}                                       \
		SELF.update = __csr_update__##SELF;     \

/*-----------------------------------------------------------------------------
	__csr_delete__ () constrói uma instrução DELETE e a executa a partir das
	informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
-----------------------------------------------------------------------------*/
	int __csr_delete__ (csrObject *self, char *table);

	#define __CSR_DELETE__(SELF)               \
		int __csr_delete__##SELF (char *table)  \
		{                                       \
			return __csr_delete__(&SELF, table); \
		}                                       \
		SELF.delete = __csr_delete__##SELF;     \




/*-----------------------------------------------------------------------------
	__csr_drop__ () constrói uma instrução DROP TABLE
	table: string contendo o nome da tabela
-----------------------------------------------------------------------------*/
	int __csr_drop__ (csrObject *self, char *table);

	#define __CSR_DROP__(SELF)               \
		int __csr_drop__##SELF (char *table)  \
		{                                     \
			return __csr_drop__(&SELF, table); \
		}                                     \
		SELF.drop = __csr_drop__##SELF;       \

/*-----------------------------------------------------------------------------
	__csr_create__ () constrói uma instrução CREATE TABLE e a executa a partir
	das informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
-----------------------------------------------------------------------------*/
	int __csr_create__ (csrObject *self, char *table);

	#define __CSR_CREATE__(SELF)               \
		int __csr_create__##SELF (char *table)  \
		{                                       \
			return __csr_create__(&SELF, table); \
		}                                       \
		SELF.create = __csr_create__##SELF;     \

/*-----------------------------------------------------------------------------
	__csr_select__ () constrói uma instrução SELECT e a executa a partir das
	informações de registros adicionados pelo método add()
	table: string contendo o nome da tabela
	reader: função a executar durante a pesquisa (opcional)
-----------------------------------------------------------------------------*/
	int __csr_select__ (csrObject *self, char *table, void (*reader)());

	#define __CSR_SELECT__(SELF)                                \
		int __csr_select__##SELF (char *table, void (*reader)()) \
		{                                                        \
			return __csr_select__(&SELF, table, reader);          \
		}                                                        \
		SELF.select = __csr_select__##SELF;                      \

/*-----------------------------------------------------------------------------
	__csr_add__ () adiciona/altera registros para execução dos métodos
	construtores de instruções SQL
	column: string contendo o nome da coluna
	value:  string contendo o valor da coluna
-----------------------------------------------------------------------------*/
	int __csr_add__ (csrObject *self, char *column, char *value, int where);

	#define __CSR_ADD__(SELF)                                       \
		int __csr_add__##SELF (char *column, char *value, int where) \
		{                                                            \
			return __csr_add__(&SELF, column, value, where);          \
		}                                                            \
		SELF.add = __csr_add__##SELF;                                \

/*-----------------------------------------------------------------------------
	__csr_clear__ () limpa os dados adicionados pelo método add
-----------------------------------------------------------------------------*/
	int __csr_clear__ (csrObject *self);

	#define __CSR_CLEAR__(SELF)          \
		int __csr_clear__##SELF ()        \
		{                                 \
			return __csr_clear__(&SELF);   \
		}                                 \
		SELF.clear = __csr_clear__##SELF; \

/*-----------------------------------------------------------------------------
	__csr_fetch__ () retorna a string com o valor da coluna na instrução SELECT
	Obs.: Retorna NULL se a coluna é nula, não foi encontrada ou quando ocorre
	um erro
-----------------------------------------------------------------------------*/
	char *__csr_fetch__ (csrObject *self, char *col);

	#define __CSR_FETCH__(SELF)              \
		char *__csr_fetch__##SELF (char *col) \
		{                                     \
			return __csr_fetch__(&SELF, col);  \
		}                                     \
		SELF.fetch = __csr_fetch__##SELF;     \

/*-----------------------------------------------------------------------------
	__csr_status__ () retorna o status da última solicitação
-----------------------------------------------------------------------------*/
	int __csr_status__ (csrObject *self);

	#define __CSR_STATUS__(SELF)           \
		int __csr_status__##SELF ()         \
		{                                   \
			return __csr_status__(&SELF);    \
		}                                   \
		SELF.status = __csr_status__##SELF; \

/*-----------------------------------------------------------------------------
	__csr_info__ () retorna a menssagem da última solicitação
-----------------------------------------------------------------------------*/
	char *__csr_info__ (csrObject *self);

	#define __CSR_INFO__(SELF)         \
		char *__csr_info__##SELF ()     \
		{                               \
			return __csr_info__(&SELF);  \
		}                               \
		SELF.info = __csr_info__##SELF; \

/*-----------------------------------------------------------------------------
	__csr_debug__ () liga/desliga depuração
-----------------------------------------------------------------------------*/
	void __csr_debug__ (csrObject *self, int val);

	#define __CSR_DEBUG__(SELF)             \
		void __csr_debug__##SELF (int val)   \
		{                                    \
			return __csr_debug__(&SELF, val); \
		}                                    \
		SELF.debug = __csr_debug__##SELF;    \


/*-----------------------------------------------------------------------------
	new_CSR () construtor da estrutura
-----------------------------------------------------------------------------*/
	#define new_CSR(OBJECT, FILE) \
                                 \
		csrObject OBJECT;		      \
		OBJECT.file    = FILE;     \
		OBJECT.message = "";       \
		OBJECT.code    = CSR_OK;   \
		OBJECT.print   = 1;        \
		OBJECT.row     = 0;        \
		OBJECT.len     = 0;        \
		OBJECT.data    = NULL;     \
		OBJECT.col     = NULL;     \
		OBJECT.val     = NULL;     \
		OBJECT.reader  = NULL;     \
                                 \
		__CSR_SQL__(OBJECT);       \
		__CSR_INSERT__(OBJECT);    \
		__CSR_REPLACE__(OBJECT);   \
		__CSR_UPDATE__(OBJECT);    \
		__CSR_DELETE__(OBJECT);    \
		__CSR_SELECT__(OBJECT);    \
		__CSR_CREATE__(OBJECT);    \
		__CSR_DROP__(OBJECT);      \
		__CSR_ADD__(OBJECT);       \
		__CSR_CLEAR__(OBJECT);     \
		__CSR_FETCH__(OBJECT);     \
		__CSR_STATUS__(OBJECT);    \
		__CSR_INFO__(OBJECT);      \
		__CSR_DEBUG__(OBJECT);     \

#endif
