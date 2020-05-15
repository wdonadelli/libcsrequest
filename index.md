# SQLite Request Library - CSR

SCR is a library written in C language designed to simplify requests with an [SQLite](https://www.sqlite.org/) database.

The interaction with the database occurs through a structure (type) that points to attributes and methods responsible for the requests.

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

It is a shortcut function to create a simplified *SELECT* statement from the data registered in the `add` method.

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

```sql
SELECT * FROM tab;
```

```c
varDB.add("age", NULL, 0);
varDB.add("name", NULL, 0);

varDB.select("tab", myReader);
```

```sql
SELECT name, age FROM tab;
```

```c
varDB.add("age", NULL, 0);
varDB.add("name", NULL, 0);
varDB.add("age", "12", 1); /* where enabled */

varDB.select("tab", myReader);
```

```sql
SELECT name, age FROM tab WHERE age = '12';
```

```c
varDB.add("age", NULL, 0);
varDB.add("name", NULL, 0);
varDB.add("age", "12", 1); /* It will be ignored by the next command. */
varDB.add("name", NULL, 1); /* where enabled (last) */

varDB.select("tab", myReader);
```

```sql
SELECT name, age FROM tab WHERE name IS NULL;
```

<!--.........................................................................-->

### insert

It is a shortcut function to create a simplified *INSERT* statement from the data registered in the `add` method.

**For more complex declarations, use the `sql` method.**

```c
int insert (char *table)
```

#### The Records

All records with the `where` argument disabled will form the _key/value_ set defined in the _INSERT_ statement.

#### Parameters

|Name|Description|
|:--|:--|
|table|(required) Table name.|

##### Returns

`CSR_OK`, `CSR_ERR` or `CSR_FAIL`.

#### Examples

```c
varDB.add("age", "13", 0);
varDB.add("name", NULL, 1); /* will be ignored */

varDB.insert("tab");
```

```sql
INSERT INTO tab (age) VALUES ('13');

```

```c
varDB.add("age", "13", 0);
varDB.add("name", NULL, 0);

varDB.insert("tab");
```

```sql
INSERT INTO tab (name, age) VALUES (NULL, '13');

```

<!--.........................................................................-->

### update

It is a shortcut function to create a simplified *UPDATE* statement from the data registered in the `add` method.

**For more complex declarations, use the `sql` method.**

```c
int update (char *table)
```

#### The Records

All records with the `where` argument disabled will form the column list in the *UPDATE* statement. If none are found, a `CSR_ERR` will return.

Only the most recent record with the `where` argument enabled will be used in the *WHERE* statement for the purpose of **comparing equality**. If none are found, a `CSR_ERR` will return (to avoid disasters).

#### Parameters

|Name|Description|
|:--|:--|
|table|(required) Table name.|

##### Returns

`CSR_OK`, `CSR_ERR` or `CSR_FAIL`.

#### Examples

```c
varDB.add("age", "25", 0);
varDB.add("name", "Maria", 0);
varDB.add("age", "12", 1); /* It will be ignored by the next command. */
varDB.add("name", "Mary", 1); /* where enabled (last) */

varDB.update("tab");
```

```sql
UPDATE tab SET name = 'Maria', age = '25' WHERE name = 'Mary';
```

<!--.........................................................................-->

### delete

It is a shortcut function to create a simplified *DELETE* statement from the data registered in the `add` method.

**For more complex declarations, use the `sql` method.**

```c
int delete (char *table)
```

#### The Records

All records with the `where` argument disabled will be ignored.

Only the most recent record with the `where` argument enabled will be used in the *WHERE* statement for the purpose of **comparing equality**. If none are found, a `CSR_ERR` will return (to avoid disasters).

#### Parameters

|Name|Description|
|:--|:--|
|table|(required) Table name.|

##### Returns

`CSR_OK`, `CSR_ERR` or `CSR_FAIL`.

#### Examples

```c
varDB.add("age", "25", 0); /* It will be ignored */
varDB.add("name", "Maria", 0); /* It will be ignored */
varDB.add("age", "12", 1); /* It will be ignored by the next command. */
varDB.add("name", NULL, 1); /* where enabled (last) */

varDB.delete("tab");
```

```sql
DELETE FROM tab WHERE name IS NULL;
```

## Additional Notes

### Source Code

- [libcsrequest.h](https://wdonadelli.github.io/libcsrequest/libcsrequest.h)
- [libcsrequest.c](https://wdonadelli.github.io/libcsrequest/libcsrequest.c)

### Package

- *libsqlite3-dev* package or equivalent.

### Compilation (GCC) 

To compile the source code it is necessary to use the `-l sqlite3` flag.

```sh
gcc -c libcsrequest.c -l sqlite3
```

### Versions

#### v1.0.0 (2020-05-14)

- Initial release.

### Authors

- Willian Donadelli (<wdonadelli@gmail.com>)
