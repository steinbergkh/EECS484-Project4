EECS 484 -- Project 4
================
##Minirel 2000 Query Processor

###Introduction
Implementing a query processor for a simple single-user DBMS that accepts a (small) subset of SQL.

###Framework Overview
The following is a brief description of each of the main components.

<dl>
  <dt>Parser</dt>
  <dd>The **minirel** executable accepts SQL queries and other utility commands. (See the next section for a full description of the supported SQL commands.) We provide a parser, which first parses the input SQL, and then consults the system catalogs to make sure the commands are valid (i.e., the relations and attributes mentioned by the command actually exist in the database.) If the SQL is valid, the parser calls the appropriate query operators and utilities. The parser is implemented for you.
</dd>

  <dt>Query Optimizer, Operators, and Utilities</dt>
  <dd>If the incoming SQL command is valid, the parser calls the appropriate function to process the command:
  * If the command is a utility, the parser calls the appropriate function to process the utility. For this project, you will only be implementing one utility (insert). The function header (Updates::Insert) can be found in the file query.h, and the actual implementation of this function will go in insert.cpp.
  * If the command is a query, the parser first determines if the query is a select query (referencing just one table) or a join query. To execute a select query, the parser calls Operators::Select (in select.cpp). To execute a join query, it calls Operators::Join (in join.cpp). The Operator class definition can be found in query.h. </dd>

  <dt>Storage Manager</dt>
  <dd>Beneath the query processor, there are two main access methods for data: indexes and heapfiles. From the query processor, you will need to implement public methods. To understand these classes and methods, it should be sufficient to look at the header files (heapfile.h and index.h).</dd>
  
  <dt>System Catalogs</dt>
  <dd>Recall that the system catalogs are used to store metadata (“data about the data”), including the names of all tables, and the names and types of all attributes. To understand the classes and methods, it should be sufficient to look at the header file (catalog.h).</dd>
</dl>
