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

Compilation with gcc:
FIXME gcc `pkg-config --cflags gtk+-3.0` -c libuigtk.c `pkg-config --libs gtk+-3.0`;

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
	Estrutura do retorno de pesquisas
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
		int (*sql)();
		int (*insert)();
		int (*update)();
		int (*delete)();
		int (*view)();
		int (*add)();
		int (*clear)();
		
		/*-- Atributos --*/
		char *file;            /* nome do arquivo */
		unsigned int error: 1; /* informa se houve erro na última operação */
		char *msg;             /* informa mensagem de erro da última operação */
		csrData *data;         /* lista de inclusões */
		//csrData **query; /* [linhas][colunas] */
	} csrObject;
	
/*-----------------------------------------------------------------------------
	Protótipos

	Os protótipos abaixo não devem ser utilizadas para manipulação da
	biblioteca, sua utilidade se destina a criação do métodos secundários da
	ferramenta principal.
	Todos os protótipos retornam 1 (sucesso) ou 0 (erro)
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	__csr_sql__ () executa uma requisição a partir de uma estrutura SQL
-----------------------------------------------------------------------------*/
	int __csr_sql__ (csrObject *self, char *query, void (*reader)());

	#define __CSR_SQL__(SELF) \
		int __csr_sql__##SELF (char *query, void (*reader)()) { \
			return __csr_sql__(&SELF, query, reader); \
		} \
		SELF.sql = __csr_sql__##SELF;

/*-----------------------------------------------------------------------------
	__csr_insert__ () executa uma inserção a partir de uma lista de estrutura
-----------------------------------------------------------------------------*/
	int __csr_insert__ (csrObject *self, char *table);

	#define __CSR_INSERT__(SELF) \
		int __csr_insert__##SELF (char *table) { \
			return __csr_insert__(&SELF, table); \
		} \
		SELF.insert = __csr_insert__##SELF;

/*-----------------------------------------------------------------------------
	__csr_update__ () executa uma alteração a partir de uma lista de estrutura
-----------------------------------------------------------------------------*/
	int __csr_update__ (csrObject *self, char *table);

	#define __CSR_UPDATE__(SELF) \
		int __csr_update__##SELF (char *table) { \
			return __csr_update__(&SELF, table); \
		} \
		SELF.update = __csr_update__##SELF;

/*-----------------------------------------------------------------------------
	__csr_delete__ () executa uma deleção a partir de uma lista de estrutura
-----------------------------------------------------------------------------*/
	int __csr_delete__ (csrObject *self, char *table);

	#define __CSR_DELETE__(SELF) \
		int __csr_delete__##SELF (char *table) { \
			return __csr_delete__(&SELF, table); \
		} \
		SELF.delete = __csr_delete__##SELF;

/*-----------------------------------------------------------------------------
	__csr_view__ () executa uma pesquisa a partir de uma lista de estrutura
-----------------------------------------------------------------------------*/
	int __csr_view__ (csrObject *self, char *table, void (*reader)());

	#define __CSR_VIEW__(SELF) \
		int __csr_view__##SELF (char *table, void (*reader)()) { \
			return __csr_view__(&SELF, table, reader); \
		} \
		SELF.view = __csr_view__##SELF;

/*-----------------------------------------------------------------------------
	__csr_add__ () adiciona dados para execução dos métodos acima (-sql)
-----------------------------------------------------------------------------*/
	int __csr_add__ (csrObject *self, char *column, char *value, int where);

	#define __CSR_ADD__(SELF) \
		int __csr_add__##SELF (char *column, char *value, int where) { \
			return __csr_add__(&SELF, column, value, where); \
		} \
		SELF.add = __csr_add__##SELF;

/*-----------------------------------------------------------------------------
	__csr_clear__ () limpa os dados adicionados pelo método add
-----------------------------------------------------------------------------*/
	int __csr_clear__ (csrObject *self);

	#define __CSR_CLEAR__(SELF) \
		int __csr_clear__##SELF () { \
			return __csr_clear__(&SELF); \
		} \
		SELF.clear = __csr_clear__##SELF;

/*-----------------------------------------------------------------------------
	new_CSrequest () construtor da estrutura
-----------------------------------------------------------------------------*/
	#define new_CSrequest(OBJECT, FILE) \
	\
		csrObject OBJECT; \
		OBJECT.file = malloc ((strlen(FILE) + 1) * sizeof (char)); \
		strcpy(OBJECT.file, FILE); \
		OBJECT.error = 0; \
		OBJECT.data = NULL; \
		__CSR_SQL__(OBJECT); \
		__CSR_INSERT__(OBJECT); \
		__CSR_UPDATE__(OBJECT); \
		__CSR_DELETE__(OBJECT); \
		__CSR_VIEW__(OBJECT); \
		__CSR_ADD__(OBJECT); \
		__CSR_CLEAR__(OBJECT);

#endif
