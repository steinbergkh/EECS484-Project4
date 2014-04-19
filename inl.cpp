#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "index.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>


bool streq( const char * str1, const char * str2 ){
   int retVal = strcmp(str1, str2);
   if (retVal == 0){
      return true;
   }
   else{
      return false;
   }
}


/*
* Indexed nested loop evaluates joins with an index on the
* inner/right relation (attrDesc2)
* NOTE BY KT: this is accounted for in join.cpp, so we only
*             have to evaluate the join on the right index
*/

Status Operators::INL(const string& result,           // Name of the output relation
                     const int projCnt,              // Number of attributes in the projection
                     const AttrDesc attrDescArray[], // The projection list (as AttrDesc)
                     const AttrDesc& attrDesc1,      // The left attribute in the join predicate
                     const Operator op,              // Predicate operator
                     const AttrDesc& attrDesc2,      // The left attribute in the join predicate
                     const int reclen)               // Length of a tuple in the output relation
{
   cout << "Algorithm: Indexed NL Join" << endl;
   cout << "reclen = " << reclen << endl;
   /* Your solution goes here */

   Status status;

   HeapFile *heapFile = new HeapFile(result, status);

   // get Datatypes of joined attributes
   Datatype attrType1 = (Datatype)attrDesc1.attrType;
   Datatype attrType2 = (Datatype)attrDesc2.attrType;

   string leftRelName = attrDesc1.relName;  // get the relation name of the left attr
   string rightRelName = attrDesc2.relName; // get the relation name of the right attr

   HeapFileScan *leftFileScan = new HeapFileScan(leftRelName, status);
   leftFileScan->startScan(-1,          // if offset < 0  return bad scan param error
                           0,           // if length < 1  return bad scan param error
                           (Datatype)0, // do we have to static cast this?
                           NULL,        // no filtering requested, should make function return okay
                           (Operator)0);

   if (status != OK){
      delete heapFile;
      heapFile= NULL;
      delete leftFileScan;
      leftFileScan = NULL;
      return status;
   }





   Index *attrIndex = new Index(rightRelName,        // name of the relation being indexed
                              attrDesc2.attrOffset, // offset of the attribute being indexed
                              attrDesc2.attrLen,    // length of the attribute being indexed
                              attrType2,             // type of the attribute being indexed
                              0,                     // =1 if the index should only allow unique entries.
                              status);               // return error codes

   if (status != OK){
      delete heapFile;
      heapFile = NULL;
      delete leftFileScan;
      leftFileScan = NULL;
      delete attrIndex;
      attrIndex = NULL;
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
         delete attrIndex;
         attrIndex = NULL;
         return OK;
      }

      /*  -------------------------------------------
          ------   INDEX SCAN ON RIGHT ATTR    ------
          -------------------------------------------  */

      void *indexAttrVal = malloc(attrDesc1.attrLen);
      cout << "attempting to copy mem into temp var for index search" << endl;
      memcpy(indexAttrVal, leftRecord.data + attrDesc1.attrOffset, attrDesc1.attrLen);
      cout << "\t index search on record " <<

      HeapFileScan *heapFileScan2 = new HeapFileScan(attrDesc2.relName,
                                                 attrDesc2.attrOffset,
                                                 attrDesc2.attrLen,
                                                 attrType2,
                                                 (char *)indexAttrVal,
                                                 op,
                                                 status);

      // start index scan for char* indexAttrVal; = (char *)leftRecord.data + attrDesc1.attrOffset
      status = attrIndex->startScan(indexAttrVal);
      if (status != OK){ // this means there weren't any records matching this
         attrIndex->endScan();
         delete heapFileScan2;
         heapFileScan2 = NULL;
         return OK;
      }
      // breaks when it's done finding all the records on the left
      // that match this index
      while(true){
         // grab the next record
         status = attrIndex->scanNext(rightRID);

         if (status != OK){ // this means there wasn't a next record in this index to grab
            attrIndex->endScan();
            delete heapFileScan2;
            heapFileScan2 = NULL;
            break;
         }

         heapFileScan2->getRandomRecord(rightRID, rightRecord);

         if (status != OK){ // this means there wasn't a record to grab
            attrIndex->endScan();
            free(resultRecord.data);
            delete heapFile;
            heapFile = NULL;
            delete heapFileScan2;
            heapFileScan2 = NULL;
            delete leftFileScan;
            leftFileScan = NULL;
            return OK;
         }

         resultRecord.data = malloc(reclen); // allocate enough room for all our shtuff

         resultRecOffset = 0;
         for (int i = 0; i < projCnt ; ++i){
            if (streq(attrDescArray[i].relName, attrDesc1.relName)){
               cout << "attempting to copy mem into result record from LEFT attr" << endl;
               // this attr in result record comes from relation of the left attr
               memcpy(resultRecord.data + resultRecOffset, // should point to end of last attr in new record
                        leftRecord.data + attrDescArray[i].attrOffset,
                        attrDescArray[i].attrLen);
            }
            else{ // this attr in result record comes from relation of the right attr
               cout << "attempting to copy mem into result record from RIGHT attr" << endl;
               cout << "(char*)rightRecord.data" << (char*)rightRecord.data << endl;
               cout << "(char*)(rightRecord.data + attrDescArray[i].attrOffset)" << *(rightRecord.data + attrDescArray[i].attrOffset) << endl;
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
            attrIndex->endScan();
            free(resultRecord.data);
            delete heapFile;
            heapFile = NULL;
            delete leftFileScan;
            leftFileScan = NULL;
            delete heapFileScan2;
            heapFileScan2 = NULL;
            return status;
         }
      }
   }

   return OK;
}
