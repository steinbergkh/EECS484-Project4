#include "catalog.h"
#include "query.h"
#include "index.h"
#include "string.h"
#include "stdlib.h"

Status Operators::IndexSelect(const string& result,       // Name of the output relation
                              const int projCnt,          // Number of attributes in the projection
                              const AttrDesc projNames[], // Projection list (as AttrDesc)
                              const AttrDesc* attrDesc,   // Attribute in the selection predicate
                              const Operator op,          // Predicate operator
                              const void* attrValue,      // Pointer to the literal value in the predicate
                              const int reclen)           // Length of a tuple in the output relation
{
  cout << "Algorithm: Index Select" << endl;

  /* Your solution goes here */
  Status status;

  HeapFile *heapFile = new HeapFile(result, status);
  Datatype projAttrType = (Datatype)attrDesc->attrType;
  char *projAttrVal = (char*)attrValue;  //do we have to static cast this?
  HeapFileScan *heapFileScan = new HeapFileScan(attrDesc->relName, attrDesc->attrOffset, attrDesc->attrLen,
                                  projAttrType, projAttrVal, op, status);

  if (status != OK){
     delete heapFile;
     delete heapFileScan;
     return status;
  }

  Datatype indexDatatype = Datatype(attrDesc->attrType); // static cast attrType                                                      // from int to Datatype
  string indexRelName = attrDesc->relName;
  Index *attrIndex = new Index(indexRelName,    // name of the relation being indexed
                        attrDesc->attrOffset,   // offset of the attribute being indexed
                        attrDesc->attrLen,      // length of the attribute being indexed
                        indexDatatype,          // type of the attribute being indexed
                        0,                      // =1 if the index should only allow unique entries.
                        status);        // return error codes

   if (status != OK){
      delete heapFile;
      delete heapFileScan;
      return status;
   }
   // this func is only called if there is an index on the relation "indexRelName"
   // which means the constructor should find the file and put the header page
   // into the buffer poo

   RID nextRID, resultRID;
   Record nextRecord, resultRecord;
   int resultRecOffset;

   status = attrIndex->startScan(projAttrVal);
   if (status != OK){ // this means there wasn't a next record to grab
      attrIndex->endScan();
      return OK;
   }

   while(true){
      // grab the next record
      status = attrIndex->scanNext(nextRID);

      if (status != OK){ // this means there wasn't a next record to grab
         delete heapFile;
         delete heapFileScan;
         attrIndex->endScan();
         return OK;
      }

      heapFileScan->getRandomRecord(nextRID, nextRecord);

      resultRecord.data = malloc(reclen); // allocate enough room for all our shtuff

      resultRecOffset = 0;
      for (int i = 0; i < projCnt ; ++i){
         memcpy(resultRecord.data + resultRecOffset, // should point to end of last attr in new record
               nextRecord.data + projNames[i].attrOffset,
               projNames[i].attrLen);
         resultRecOffset += projNames[i].attrLen;
      }
      resultRecord.length = resultRecOffset; // should be equal to the val of
                                            // the last offset plus it's length
      heapFile->insertRecord(resultRecord, resultRID);
      if (status != OK){ // this means there was an issue
         delete heapFile;
         delete heapFileScan;
         return status;
      }
   }

   return OK;
}
