EECS 484 -- Project 4
================
##Minirel 2000 Query Processor

###Introduction
Implementing a query processor for a simple single-user DBMS that accepts a (small) subset of SQL.

###Supported SQL Statements and Parser
The SQL parser that we provide understands only a small subset of standard SQL:
<dl>
  <dt>CREATE TABLE TBLNAME (ATTRLIST...)</dt>
  <dd>Note that the parser does not support any key or table constraints. Supported data types include INTEGER, DOUBLE and CHAR; no attribute values can be null. For example, the following statement creates a table with three attributes:
  ``CREATE TABLE DA (IKEY INTEGER, FILLER CHAR(80), DKEY DOUBLE)``</dd>
  
  <dt>CREATE INDEX <i>RelName</i> (AttrName)</dt>
  <dd>As you may recall, SQL has no standard command for creating an index. This statement creates a hash Index in Minirel2K. The code for <b>hash index</b> is provided (see files index.h and index.cpp).</dd>
  
  <dt>DROP TABLE <i>RelName</i></dt>
  <dd>Deletes the table <i>RelName</i></dd>
  
  <dt>DROP INDEX <i>RelName</i> (AttrName)</dt>
  <dd>Deletes the index on the <i>AttrName</i> attribute of the relation <i>RelName</i></dd>
  
  <dt>INSERT INTO <i>RelName</i> (AttrNameList) VALUES (ValueList) </dt>
  <dd>The standard SQL “insert into” command. The only difference is that the AttrNameList is mandatory (not optional as in SQL). For example, the following statement inserts a tuple into the DA table: INSERT INTO DA (IKEY, FILLER, DKEY) VALUES (11, ‘SING A SONG’’, 111.0); 
  
  When the parser detects an insert command, it calls the “Updates::Insert” method (see file query.h and insert.cpp).<b> Values for doubles in all Minirel2K SQL must be specified in the form “Num.Num”.</b> Note that in the insert into statement above the value for dkey is specified as 111.0 and not 111.</dd>
  
  <dt>SELECT PROJLIST FROM RELLIST WHERE PREDICATE:</dt>
  <dd>The SQL command for querying.Only a limited class of queries is supported. First, the RelList can have at most two relations. Second, the Predicate can only be a single predicate (no ANDs or ORs). Third, all attributes must be referenced using the form “RelName.AttrName”, not just “AttrName”. Fourth, all literal values for doubles must be in the form “Num.Num”. Finally, there are no nested select statements.
  
  Put another way, the following two types of queries are supported:
  
  *<b>SELECT ProjJList FROM RelName WHERE RelName.ATTR OP Literal</b><br>A simple select query on a single table. When the parser detects a select query, it calls the “Operators::Select” method (see file query.h and select.cpp).
  *<b>SELECT ProjJList<br>FROM RelName1, RelName2<br>WHERE RelName1.ATTR1 OP RelName2.ATTR2</b><br> A simple join query on two tables. When the parser detects this join query, it calls the “Operators::Join” method (see file query.h and join.cpp).</dd>
  
  <dt>QUIT</dt>
  <dd>The quit command exits the Minirel2K system.</dd>
</dl>

###Framework Overview
The following is a brief description of each of the main components.

<dl>
  <dt>Parser</dt>
  <dd>The <b>minirel</b> executable accepts SQL queries and other utility commands. (See the next section for a full description of the supported SQL commands.) We provide a parser, which first parses the input SQL, and then consults the system catalogs to make sure the commands are valid (i.e., the relations and attributes mentioned by the command actually exist in the database.) If the SQL is valid, the parser calls the appropriate query operators and utilities. The parser is implemented for you.
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
