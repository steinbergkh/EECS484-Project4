#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "index.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>


// I hate the strcmp function more than anything
// WHY DOES IT RETURN A 0 WHEN TWO STRINGS ARE EQUAL!?!?!?
bool streq( const char * str1, const char * str2 ){
   int retVal = strcmp(str1, str2);
   if (retVal == 0){
      return true;
   }
   else{
      return false;
   }
}

Status Operators::SNL(const string& result,           // Output relation name
                      const int projCnt,              // Number of attributes in the projection
                      const AttrDesc attrDescArray[], // Projection list (as AttrDesc)
                      const AttrDesc& attrDesc1,      // The left attribute in the join predicate
                      const Operator op,              // Predicate operator
                      const AttrDesc& attrDesc2,      // The left attribute in the join predicate
                      const int reclen)               // The length of a tuple in the result relation
{
  cout << "Algorithm: Simple NL Join" << endl;

  /* Your solution goes here */
  Status status;

  HeapFile *heapFile = new HeapFile(result, status);

   if (status != OK){
      delete heapFile;
      heapFile= NULL;
      return status;
   }

  // get Datatypes of joined attributes
  Datatype attrType1 = (Datatype)attrDesc1.attrType;
  Datatype attrType2 = (Datatype)attrDesc2.attrType;

  string leftRelName = attrDesc1.relName;  // get the relation name of the left attr
  string rightRelName = attrDesc2.relName; // get the relation name of the right attr

  HeapFileScan *leftFileScan = new HeapFileScan(leftRelName,
  													        attrDesc1.attrOffset,
                                  					attrDesc1.attrLen,
                                  					attrType1,
                                  					NULL,
                                  					op,
                                  					status);
  if (status != OK){
     delete heapFile;
     heapFile = NULL;
     delete leftFileScan;
     leftFileScan = NULL;
     return status;
  }
  // this func is only called if there is an index on the relation "indexRelName"
  // which means the constructor should find the file and put the header page
  // into the buffer poo

  RID leftRID, rightRID, resultRID;
  Record leftRecord, rightRecord, resultRecord;
  int resultRecOffset;



  while(true){
     // grab the next record
     status = leftFileScan->scanNext(leftRID, leftRecord);

     if (status != OK){ // this means there wasn't a next record to grab
        leftFileScan->endScan();
        delete heapFile;
        heapFile= NULL;
        delete leftFileScan;
        leftFileScan = NULL;
        return OK;
     }

     /*  -------------------------------------------
         ------   INDEX SCAN ON RIGHT ATTR    ------
         -------------------------------------------  */

     char *indexAttrVal = new char[attrDesc1.attrLen];
     char *leftRecVal = ((char*)leftRecord.data) + attrDesc1.attrOffset;
     memcpy(indexAttrVal, leftRecVal, attrDesc1.attrLen);

     HeapFileScan *rightFileScan = new HeapFileScan(rightRelName,
                                                    attrDesc2.attrOffset,
                                                    attrDesc2.attrLen,
                                                    attrType2,
                                                    leftRecVal,
                                                    op,
                                                    status);

     // start index scan for char* indexAttrVal; = (char *)leftRecord.data + attrDesc1.attrOffset
     if (status != OK){ // this means there was a bad scan param
        rightFileScan->endScan();
        delete heapFile;
        heapFile = NULL;
        delete leftFileScan;
        leftFileScan = NULL;
        delete rightFileScan;
        rightFileScan = NULL;
        return OK;
     }
     // breaks when it's done finding all the records on the left
     // that match this index
     while(true){
        // grab the next record
        status = rightFileScan->scanNext(rightRID, rightRecord);

        if (status != OK){ // this means there wasn't a next record in this index to grab
           rightFileScan->endScan();
           delete rightFileScan;
           rightFileScan = NULL;
           break;
        }

         resultRecord.data = malloc(reclen); // allocate enough room for all our shtuff


        if (status != OK){ // this means there wasn't a record to grab
           rightFileScan->endScan();
           free(resultRecord.data);
           delete heapFile;
           heapFile = NULL;
           delete rightFileScan;
           rightFileScan = NULL;
           delete leftFileScan;
           leftFileScan = NULL;
           return OK;
        }


        resultRecOffset = 0;
        for (int i = 0; i < projCnt ; ++i){
           if (streq(attrDescArray[i].relName, attrDesc1.relName)){
              // this attr in result record comes from relation of the left attr
              memcpy(resultRecord.data + resultRecOffset, // should point to end of last attr in new record
                       leftRecord.data + attrDescArray[i].attrOffset,
                       attrDescArray[i].attrLen);
           }
           else{ // this attr in result record comes from relation of the right attr
              memcpy(resultRecord.data + resultRecOffset, // should point to end of prev attr in new record
                       rightRecord.data + attrDescArray[i].attrOffset,
                       attrDescArray[i].attrLen);
           }
           resultRecOffset += attrDescArray[i].attrLen;
        }

        resultRecord.length = resultRecOffset; // should be equal to the val of
                                               // the last offset plus it's length

        heapFile->insertRecord(resultRecord, resultRID);
        if (status != OK){ // this means there was an issue
           rightFileScan->endScan();
           free(resultRecord.data);
           delete heapFile;
           heapFile = NULL;
           delete leftFileScan;
           leftFileScan = NULL;
           delete rightFileScan;
           rightFileScan = NULL;
           return status;
        }
     }
  }

  return OK;
}
