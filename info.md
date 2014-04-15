#Overview of Given Code
##Types

###attrInfo
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
###RelDesc
```cpp
// schema of relation catalog:
//   relation name : char(32)           <-- lookup key
//   attribute count : integer(4)
//   index count : integer(4)
typedef struct {
  char relName[MAXNAME];                // relation name
  int attrCnt;                          // number of attributes
  int indexCnt;                         // number of indexed attrs
} RelDesc;
```

###AttrDesc
```cpp
// schema of attribute catalog:
//   relation name : char(32)           <-- lookup keys
//   attribute name : char(32)          <--
//   attribute number : integer(4)
//   attribute type : integer(4)  (type is Datatype actually)
//   attribute size : integer(4)
//   index flag : integer(4)
typedef struct {
  char relName[MAXNAME];                // relation name
  char attrName[MAXNAME];               // attribute name
  int attrOffset;                       // attribute offset
  int attrType;                         // attribute type
  int attrLen;                          // attribute length
  int indexed;                          // TRUE if indexed
} AttrDesc;
```


###Record
```cpp
// Record - pointer to start of tuple and length of the record
struct Record{
  void* data;
  int length;
};
```
##Enums

###DataType
```cpp
// These are the data types that minirel understands
/*
enum DataType {
      SQLChar,      // SQL Char data type
      SQLInteger,   // SQL Integer data type
      SQLDouble     // SQL double data type
   };
*/

// Given out in part2
enum Datatype {INTEGER=0, DOUBLE=1, STRING=2 };
```

###Operator
```cpp
// A list of operations that are supported in predicates in minirel.
enum Operator { LT, LTE, EQ, GTE, GT, NE, NOTSET };  // scan operators
```

###Status
```cpp
enum Status {

// no error

       OK = 0, NOTUSED1 = -999,

// File and DB errors

       BADFILEPTR, BADFILE, FILETABFULL, FILEOPEN, FILENOTOPEN,
       UNIXERR, BADPAGEPTR, BADPAGENO, FILEEXISTS,

// BufMgr and HashTable errors

       BUFMAPERROR, BUFMAPNOTFOUND, BUFFEREXCEEDED, PAGENOTPINNED,
       BADBUFFER, PAGEPINNED,

// Page errors

       NOSPACE,  NORECORDS,  ENDOFPAGE, INVALIDSLOTNO,

// HeapFile errors

       BADRID, BADRECPTR, BADSCANPARM, BADSCANID, SCANTABFULL, FILEEOF,

// Index errors
       BADINDEXPARM,

// Hash Index errors

       RECNOTFOUND, BUCKETFULL, DIROVERFLOW,
       NONUNIQUEENTRY, NOMORERECS, NOCHARIDX,


// Btree index errors

       INDEXPAGEFULL, INDEXPAGENOTFULL, BADINDEXPAGEPOSITION,
       INDEXLOADERROR, ENTRYNOTUNIQUE, EXPECTINGNONLEAFPAGE, SCANEXECUTING,
       BADINDEXSCANPARM, NOSCANEXECUTING, ENDOFINDEXSCAN, NOTFOUNDINLEAF,
       NOTFOUNDINNONLEAF, MERGEERROR,

// SortedFile errors

       BADSORTPARM, INSUFMEM,

// Catalog errors

       BADCATPARM, RELNOTFOUND, ATTRNOTFOUND,
       NAMETOOLONG, DUPLATTR, RELEXISTS, NOINDEX,
       INDEXEXISTS, ATTRTOOLONG,

// Utility errors

// Query errors

       ATTRTYPEMISMATCH, TMP_RES_EXISTS,

// do not touch filler -- add codes before it

       NOTUSED2
};
```
