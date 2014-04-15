#EECS 484 - Project 4: Minirel 2K Query Processor
##Overview of Given Code


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
