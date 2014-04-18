#Insert

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

*attrCat* is a global variable that points to the attribute table for the catalog.

```cpp
 const Status getInfo(const string & rName,
		       const string & attrName,
		       AttrDesc &record);
```

The AttrDesc struct is defined as:

```cpp
typedef struct {
  char relName[MAXNAME];                // relation name
  char attrName[MAXNAME];               // attribute name
  int attrOffset;                       // attribute offset
  int attrType;                         // attribute type
  int attrLen;                          // attribute length
  int indexed;                          // TRUE if indexed
} AttrDesc;
```

At this point, you have all the information to
1. allocate the memory for a record
2. fill it with data
3. insert it into the heapfile

####To allocate the memory for the record
- you need to know the record size
- create a Record object
- allocate the correct number of bytes (record size) for the data field of the record

####Fill it with data
- fill with data from the attrList
- use the C++ built-in function [memcpy](http://www.cplusplus.com/reference/cstring/memcpy/) to do that
- rely on the data from the RelDesc object returned by the getInfo call to determine:
   - the offsets for each attribute
   - the # of bytes to copy

####Insert the record into the heapfile
- Once the record is created, call insertRecord on the ehapfile for the relation
- FIRST, the constructor for the "relation" heapFileneeds to be called
   - creates a heapFileif one doesn't exist
   - if one exists, it opens the heapfile
- THEN, the record you created needs to be inserted
- This returns a Record ID

####Index the Record
- The Record ID you just created needs to be inserted in each index for the relation
- To determine if there is one or more index for the relation, you need to look at the catalogs
