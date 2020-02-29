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

Compilation with gcc:
FIXME gcc `pkg-config --cflags gtk+-3.0` -c libuigtk.c `pkg-config --libs gtk+-3.0`;

The library is hosted at https://github.com/wdonadelli/libcsrequest and the manual
at https://wdonadelli.github.io/libcsrequest/.

Willian Donadelli <wdonadelli@gmail.com>
-----------------------------------------------------------------------------*/

#ifndef LIBRARY_CS_REQUEST_H

	#define LIBRARY_CS_REQUEST_H

/*-----------------------------------------------------------------------------
	A presente biblioteca exige a biblioteca sqlite3 e string
-----------------------------------------------------------------------------*/
	#include <sqlite3.h>
	#include <string.h>


/*-----------------------------------------------------------------------------
	Estrutura do retorno de pesquisas
	https://www.sqlite.org/limits.html
		SQLITE_MAX_LENGTH 1000000000
		SQLITE_MAX_COLUMN 2000
		SQLITE_MAX_SQL_LENGTH 1000000
		18446744073709551616
-----------------------------------------------------------------------------*/
	typedef struct
	{
		char *col;
		char *val;
	} csrData;

/*-----------------------------------------------------------------------------
	Estrutura do objeto
-----------------------------------------------------------------------------*/
	typedef struct
	{
		/*-- Métodos --*/
		int (*sql)(*csrObject, char);
		int (*insert)(*csrObject);
		int (*update)(*csrObject, *char, *char);
		int (*delete)(*csrObject, *char, *char);
		int (*view)(*csrObject, *char, *char);
		int (*add)(*csrObject, *char, *char);
		int (*insert)(*csrObject);
		/*-- Atributos --*/
		char *file;
		unsigned int error: 1;
		char *msg;
		csrData data[][];
		csrData query[][];
	} csrObject;
	
/*-----------------------------------------------------------------------------
	Os protótipos abaixo não devem ser utilizadas para manipulação da
	biblioteca, sua utilidade se destina a criação do métodos secundários da
	ferramenta principal.
	__csr_sql__ ()    executa uma requisição a partir de uma estrutura SQL
	__csr_insert__ () executa uma inserção a partir de uma lista de estrutura
	__csr_update__ () executa uma alteração a partir de uma lista de estrutura
	__csr_delete__ () executa uma deleção a partir de uma lista de estrutura
	__csr_view__ ()   executa uma pesquisa a partir de uma lista de estrutura
	__csr_add__ ()    adiciona dados para execução dos métodos acima (-sql)
	__csr_clear__ ()  limpa os dados adicionados pelo método add
	Todos os protótipos retornam 1 (sucesso) ou 0 (erro)
-----------------------------------------------------------------------------*/
	int __csr_sql__ (*csrObject self, char *query);
	int __csr_insert__ (*csrObject self);
	int __csr_update__ (*csrObject self, char *col, char *val);
	int __csr_delete__ (*csrObject self, char *col, char *val);
	int __csr_view__ (*csrObject self, char *col, char *val);
	int __csr_add__ (*csrObject self, char *col, char *val);
	int __csr_clear__ (*csrObject self);



/*
new_CSrequest(db, "banco.db");
db.add("col1", "value1", 0);
db.add("col2", "value2", 0);
db.add("col3", "value3", 0);
db.add("col4", "value4", 0);
db.add("col5", "value5", 1);
db.add("col6", "value6", 0);
db.insert();
db.error();
db.clear();







*/

/*-----------------------------------------------------------------------------
	uigtk_init() inicia a inteface GTK a partir de um arquivo.ui (builde xml)
	Argumentos:
		file - é o nome do arquivo com a interface xml (.ui)
	Saída:
		Força o encerramento da aplicação se algum erro for encontrado
-----------------------------------------------------------------------------*/
	void uigtk_init (char *file);

/*-----------------------------------------------------------------------------
	uigtk_callback() conecta os sinais definidos diretamente na interface
	Argumentos:
		name    - nome da função a ser disparada quando o evento ocorrer
		handler - função a ser disparada quando o evento ocorrer
	Saída:
		Força o encerramento da aplicação se algum erro for encontrado
	Sugere-se o uso da macro uigtk_handler()
-----------------------------------------------------------------------------*/
	void uigtk_callback (char *name, void (*handler)());

	#define uigtk_handler(handler) uigtk_callback(#handler, handler)

/*-----------------------------------------------------------------------------
	uigtk_main() inicia o looping principal do GTK (última ação)
	Saída:
		Força o encerramento da aplicação se algum erro for encontrado
-----------------------------------------------------------------------------*/
	void uigtk_main (void);

/*-----------------------------------------------------------------------------
	uigtk_builder() retorna o ponteiro do builder para o argumento
	Argumentos:
		builder - variável a receber o ponteiro do builder
	Saída:
		Força o encerramento da aplicação se algum erro for encontrado
	Sugere-se o uso da macro uigtk_set_builder() se for associar a uma variável
		var é o nome da variável sem necessidade de definição de tipo
-----------------------------------------------------------------------------*/
	GtkBuilder *uigtk_builder (void);
	
	#define uigtk_set_builder(var) GtkBuilder *var = uigtk_builder ()

/*-----------------------------------------------------------------------------
	uigtk_object() obtém o objeto da interface a partir de seu id
	Retornos:
		NULL    - se o objeto não for encontrado
		GObject - se o objeto for encontrado
	Argumentos:
		id - identificador do objeto
	Sugere-se o uso da macro uigtk_set_object() se for associar a uma variável
		var é o nome da variável sem necessidade de definição de tipo
-----------------------------------------------------------------------------*/
	GObject *uigtk_object (char *id);

	#define uigtk_set_object(var, id) GObject *var = uigtk_object (id)

/*-----------------------------------------------------------------------------
	uigtk_dialog() exibe uma caixa de mensagem
	Retornos:
		-1 - se "não" foi clicado
		 0 - se a caixa foi fechada
		 1 - se "sim" ou "ok" forem clicados
	Argumentos:
		type - 
			0 - para caixa de informação (OK)
			1 - para caixa de alerta (OK)
			2 - para caixa de erro (OK)
			3 - para caixa de questão (SIM/NÃO)
			4 - para caixa genérica (CANCELAR)
		text - texto da mensagem
-----------------------------------------------------------------------------*/
	int uigtk_dialog (int type, char *title, char *text);
#endif
