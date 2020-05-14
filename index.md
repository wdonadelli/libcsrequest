# SQLite Request Library - CSR

SCR is a library written in C language designed to simplify requests with an [SQLite](https://www.sqlite.org/) database.

The interaction with the database occurs through a structure (type) that points to attributes and methods responsible for the requests.

To import the library use the following header:

```c
#include "libcsrequest.h"
```

To compile the source code it is necessary to use the `-l sqlite3` flag (see *libsqlite3-dev* package or equivalent).

GCC example:

```sh
gcc -c libcsrequest.c -l sqlite3
```

Next, the construction and use of the tool will be demonstrated.

## The builder

The construction of the structure is done from the macro `new_CSR`:

```c
#define new_CSR(OBJECT, FILE)
```

### Parameters

|Name|Description|
|:--|:--|
|OBJECT|Name of the variable for assigning the structure.|
|FILE|Path to the database.|

It is not necessary to define the type of the variable in advance, simply inform its name. The type to be assigned is called `csrObject`.

If the path to the database file is null or empty, the value "[:memory:](https://sqlite.org/c3ref/open.html)" will be assigned.

### Example

```c
new_CSR(varDB, "myData.db");
```

`varDB` will be the variable that will receive the structure (`csrObject` type) to interact with the database called "myData.db".

<!--.........................................................................-->

## Methods

### status

The method returns the result of the last request.

```c
int status ()
```

#### Parameters

there are no parameters.

##### Returns

Three values (integer) ​​are possible:

`CSR_OK`: The method call was successful;

`CSR_ERR`: There was a known error when calling the method; and

`CSR_FAIL`: There was an error connecting to the database or the SQL request.

<!--.........................................................................-->

### info

The method returns the message of the last request according to the type of occurrence.

```c
char *info ()
```

#### Parameters

there are no parameters.

##### Returns

Two types of returns can occur. If the request returns `CSR_OK`, a blank string is returned. In the case of return `CSR_ERR` and `CSR_FAIL`, the respective message will be returned.

<!--.........................................................................-->

### debug

Turns on/off the display of error messages returned by methods on the terminal. By default, messages on the terminal are on.

```c
void debug (int val)
```

#### Parameters

|Name|Description|
|:--|:--|
|val|(required) A Integer number that, when equal to zero, turns messages off. Other values ​​turn on the display of messages.|

##### Returns

There is no return.

<!--.........................................................................-->

### fetch

Returns the value of the column within SELECT SQL requests or equivalent. It cannot be used separately from an SQL request.

```c
char *fetch (char *col)
```

#### Parameters

|Name|Description|
|:--|:--|
|col|(required) Column name.|

##### Returns

Returns `NULL` if the column value is null, if the informed column does not exist or if the method is called outside the scope of the reader function. Otherwise, it will return the column value (*string*).

If the reported column does not exist, an error of type `CSR_ERR` will be logged. This fact can be verified by the `info` and `status` methods.

<!--.........................................................................-->

### sql

The method executes an SQL request.

```c
int sql (char *query, void (*reader)())
```

#### Parameters

|Name|Description|
|:--|:--|
|query|(required) SQL Statement.|
|reader|Function to be called when information is returned in SQL requests. It is required in the case of SELECT statements, for example, in other situations it can be `NULL` value.|

##### Returns

`CSR_OK`, `CSR_ERR` or `CSR_FAIL`.

#### Examples

Create a table called "tab" with two columns, "age" (NUMBER) and "name" (TEXT):

```c
varDB.sql("CREATE TABLE tab (age NUMBER, name TEXT;", NULL));
```

<!--.........................................................................-->

### Reader Function

The purpose of the reader function is to define the action to be performed when requesting information from the database.

It has the following prototype model:

```c
void functionName (csrObject argumentName)
```

Where *functionName* is the name of the reader function and *argumentName* is the name of the variable that will receive the object `csrObject`.

**The `fetch` method can only be accessed within the scope of the reader function.**

#### Parameters

The (required) `argumentName` will receive the same structure created by the `new_CSR` macro but with additional information inherent to the data returned from the SQL request, line by line of the 3
,2request.

The structure has the following information:

`unsigned long int row`

Returns the line number of the search (from 1).

`unsigned int len`

Returns the number of columns (from 1).

##### Returns

There is no return.

#### Examples

Define a reader function and make a *SELECT* request.

```c
void myReader (csrObject data)
{
	printf("row: %ld\n", data.row);
	printf("len: %d\n",  data.len);
	printf("name: %s\n", data.fetch("name") == NULL ? "NULL" : data.fetch("name"));
	printf("age: %s\n",  data.fetch("age") == NULL ? "NULL" : data.fetch("age"));
	
	/* Example to check if an error occurred: */
	data.fetch("color");
	if (data.status() != CSR_OK) {
		printf("Error: %s\n", data.info());
	}
}

varDB.sql("SELECT * FROM tab", myReader);
```

It is worth remembering that, if the column value is null, the `fetch` method will return a null pointer (`NULL`) and not a `"NULL"` string.

<!--.........................................................................-->

### add

It is a method designed to add or change information for later manipulation.

```c
int add (char *column, char *value, int where)
```

The method is a tool for executing shortcut methods.

For different shortcut functions, different behaviors will be applied to the reported data.

#### Parameters

|Name|Description|
|:--|:--|
|column|(required) Column name (_string_).|
|value|(required) Column value (_string_).|
|where|(required) It is an integer that defines whether the information will be treated within the _SQL WHERE_ declaration (1 to confirm the destination).|

As for the `where` argument, only one information will be treated in this way, the last one recorded.

To change an entered value, the `column` and `where` arguments must be entered with the same values ​​previously informed. It is not possible to remove specific information (see the clear method).

##### Returns

`CSR_OK`or `CSR_ERR`.

#### Examples

```c
varDB.add("age", "26", 0);
varDB.add("name", "Mary", 0);
varDB.add("name", "Mary", 1);
```

Changing the age value:

```c
varDB.add("age", "27", 0);
```

The same set of elements will only be considered if the `where` and `column` arguments are the same.

<!--.........................................................................-->

### clear

Clears the data registered by the "add" method. With each successful execution of the shortcut methods, the data will be cleared.

```c
int clear ()
```

#### Parameters

there are no parameters.

##### Returns

`CSR_OK`or `CSR_ERR`.

#### Examples

```c
varDB.clear();
```

<!--.........................................................................-->

### select

It is a shortcut function to create a *SELECT* statement from the data registered in the `add` method.

**For more complex declarations, use the `sql` method.**

```c
int select (char *table, void (*reader)())
```

#### The Records

All records with the `where` argument disabled will make up the column list in the *SELECT* statement. If none are found, the declaration will cover all columns.

Only the most recent record with the `where` argument enabled will be used in the *WHERE* statement for the purpose of **comparing equality**.

**The value of the `value` argument will only make sense if the `where` argument is turned on.**

#### Parameters

|Name|Description|
|:--|:--|
|table|(required) Table name.|
|reader|(required) Reader function.|

##### Returns

`CSR_OK`, `CSR_ERR` or `CSR_FAIL`.

#### Examples

```c
varDB.select("tab", myReader);
```
"_SELECT * FROM tab;_"

```c
varDB.add("age", NULL, 0);
varDB.add("name", NULL, 0);

varDB.select("tab", myReader);
```
"_SELECT name, age FROM tab;_"

```c
varDB.add("age", NULL, 0);
varDB.add("name", NULL, 0);
varDB.add("age", "12", 1); /* where enabled */

varDB.select("tab", myReader);
```
"_SELECT name, age FROM tab WHERE age = '12';_"

```c
varDB.add("age", NULL, 0);
varDB.add("name", NULL, 0);
varDB.add("age", "12", 1); /* It will be ignored by the next command. */
varDB.add("name", NULL, 1); /* where enabled (last) */

varDB.select("tab", myReader);
```
"_SELECT name, age FROM tab WHERE name IS NULL;_"





































### Connecting signals

To connect the signals to your handlers, you must use the `uigtk_hadler()` function, whose characteristics are described below:

```c
#define uigtk_handler(handler)
```

#### Parameters

|Name|Description|
|:--|:--|
|handler|Name of the handler function|

#### Returns

Forces the application to exit if any identifiable error is found. Possible errors are:

- Call the function before calling the uigtk_init function.

### Start the main loop

To start the main loop, use the `uigtk_main()` function, whose characteristics are described below:

```c
void uigtk_main()
```
#### Returns

Forces the application to exit if any identifiable error is found. Possible errors are:

- Call the function before calling the uigtk_init function;

At the end of the looping, a new call to the uigtk_init function will be required.

### Example

Considering that the name of the file containing the source code is called "example.c" and that it is in the same directory as the interface file "example.ui", the following sequence of commands will be necessary to build the interface.

Initially it is necessary to include the library header in our source code. Considering that both are in the same directory:

```c
#include "libuigtk.h"
```

As noted in the interface file, there are two defined handlers whose construction needs to be carried out:

```xml
<signal name="clicked" handler="hello" />
...
<signal name="clicked" handler="bye" />
```

The prototypes of the functions would look like this:

```c
void hello(GtkWidget *widget, gpointer data);
void bye(GtkWidget *widget, gpointer data);
```

To start the construction, just call the function to initialize the library indicating the interface file:

```c
uigtk_init("example.ui");
```

After initialization, the manipulators must be connected, according to the signals defined in the interface:

```c
uigtk_handler(gtk_main_quit);
uigtk_handler(hello);
uigtk_handler(bye);
```

Finally, the main loop must be initialized:

```c
uigtk_main();
```

Grouping all the information, we have:

```c
#include "libuigtk.h"

void hello(GtkWidget *widget, gpointer data);

void bye(GtkWidget *widget, gpointer data);

void main(int argc, char *argv[]) {

	/* Interface initialization */
	uigtk_init("example.ui");

	/* Connecting signals */
	uigtk_handler(gtk_main_quit);
	uigtk_handler(hello);
	uigtk_handler(bye);

	/* Start the main loop */	
	uigtk_main();

}
```

Don't forget to define the actions of the manipulators.

## Compilation

The library was tested using the following version of gcc:

> gcc (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0

To compile and run the application, the following commands were executed:

```sh
gcc `pkg-config --cflags gtk+-3.0` -c libuigtk.c `pkg-config --libs gtk+-3.0`;
gcc `pkg-config --cflags gtk+-3.0` -c example.c  `pkg-config --libs gtk+-3.0`;
gcc `pkg-config --cflags gtk+-3.0` -o example example.o libuigtk.o `pkg-config --libs gtk+-3.0`;
./example;
```

## Tools

The library also has some auxiliary functions in case you need more freedom when designing the interface.

### GTK Builder

If you need to access the pointer to the GTK Builder created by the library, the `uigtk_builder()` function must be used, whose characteristics are described below:

```c
GtkBuilder *uigtk_builder (void)
```

#### Returns

Returns the pointer to GTK Builder. If you want to link the value to a variable, alternatively, you can use the macro below:

```c
#define uigtk_set_builder(var)
```

Where `var` is intended to define the name of the variable to receive the pointer.

### GTK Object

If you need to access the pointer to any interface object ( from its "ID", the `uigtk_object()` function must be used, whose characteristics are described below:

```c
GObject *uigtk_object (char *id)
```

#### Parameters

|Name|Description|
|:--|:--|
|id|Object ID value|


#### Returns

Returns the pointer to object. If you want to link the value to a variable, alternatively, you can use the macro below:

```c
#define uigtk_set_object(var, id)
```

Where `var` is intended to define the name of the variable to receive the pointer.

### Dialog

If you need to access the pointer to any interface object ( from its "ID", the `uigtk_dialog()` function must be used, whose characteristics are described below:

```c
int uigtk_dialog (int type, char *title, char *text)
```

#### Parameters

|Name|Description|
|:--|:--|
|type|Dialog type|
|title|Box title|
|text|Textual content|

#### Types

![Interface image](images/screen2.png)

![Interface image](images/screen3.png)

![Interface image](images/screen4.png)

![Interface image](images/screen5.png)

![Interface image](images/screen6.png)

#### Returns

Returns an integer value depending on the answer:

- -1 if the option "no" is chosen;
- 0 if the box has been closed; and
- 1 if "yes" or "ok" is chosen.

## Release Notes

### v1.0.0

- Published on 2020-02-22

## Author

- Willian Donadelli (<wdonadelli@gmail.com>)
