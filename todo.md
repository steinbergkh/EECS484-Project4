#To Do

Implement important relational operators and utilities. Find class definitions in [query.h](query.h). Fill in the implementations of these operators in files:
- [ ] [insert.cpp](insert.cpp)
- [ ] [select.cpp](select.cpp)
- [ ] [join.cpp](join.cpp)
- [ ] [indexselect.cpp](indexselect.cpp)
- [ ] [scanselect.cpp](scanselect.cpp)
- [ ] [snl.cpp](snl.cpp)
- [ ] [inl.cpp](inl.cpp)
- [ ] [smj.cpp](smj.cpp)

##From "Getting Started" Section

> A good starting point is to implement the insert method first.

To insert a record into "stars", the insert method needs to:
1. Insert the record into the Heapfile corresponding to "stars"
2. Insert the record ID into each index that exists on "stars"

To insert the record, you first need to map the data from the arguments to insert into a Record object

The Record object is defined in [page.h](page.h) as:
```cpp
struct Record {
   void* data;
   int length;
}
```

The data that is passed in is in *attrList*, which is an array of *attrInfo* objects. *attrInfo* is defined in [catalog.h](catalog.h) as:
```cpp
typedef struct {
   char relName[MAXNAME];   // relation name
   char attrName[MAXNAME];  // attribute name
   int attrType;            // INTEGER, DOUBLE, or STRING
   int attrLen;             // length of attribute in bytes
   void *attrValue          // ptr to binary value (used by the
                            //    parser for insert into statements)
} attrInfo;
```

For the INSERT command:
```sql
INSERT INTO stars(starid, real_name, plays, soapid)
   VALUES (100, 'Posey, Parker', 'Tess', 6);
```
the four elements of the array will be something like the following:
```
[
   <"stars", "starid", INTEGER, -1, pointer to 4 bytes containing 100>,
   <"stars", "real_name", STRING, -1, pointer to "Posey, Parker">,
   <"stars", "plays", STRING, -1, pointer to "Tess">,
   <"stars", "soapid", INTEGER, -1, pointer to 4 bytes containing 6>,
]

```
