#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "index.h"
#include "string.h"
#include "stdlib.h"
#include "error.h"
#include <stdio.h>

MatchRecType Operators::matchRecCompare(const Record& outerRec,    // Left record
                           const Record& innerRec,    // Right record
                           const AttrDesc& attrDesc1, // Left attribute in the predicate
                           const AttrDesc& attrDesc2) // Right attribute in the predicate
{
   int cmp = matchRec(outerRec, innerRec, attrDesc1, attrDesc2);
   if (cmp == 0){ // if attributes are equal, matchRec returns 0
      return EQUAL;
   }
   else if (cmp < 0){       // if left attr is less than right attr,
      return LEFTLTRIGHT;   // matchRec returns negative number
   }
   else{ // if left attr is greater than right attr, matchRec returns positive #
      return RIGHTLTLEFT;
   }
}

/* Consider using Operators::matchRec() defined in join.cpp
* to compare records when joining the relations */

Status Operators::SMJ(const string& result,             // Output relation name
                        const int projCnt,              // Number of attributes in the projection
                        const AttrDesc attrDescArray[], // Projection list (as AttrDesc)
                        const AttrDesc& attrDesc1,      // The left attribute in the join predicate
                        const Operator op,              // Predicate operator
                        const AttrDesc& attrDesc2,      // The left attribute in the join predicate
                        const int reclen)               // The length of a tuple in the result relation
{
   cout << "Algorithm: SM Join" << endl;

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

   // gotta get some info about these records and their lengths to find out
   // how many of them can fit on a page
   // consult the system catalogs and determine the size of the tuples (in # of bytes)
   int leftAttrCnt, rightAttrCnt;
   int leftAttrLen = 0, rightAttrLen = 0;

   AttrDesc* leftAttrDesc;
   AttrDesc* rightAttrDesc;

   // get dat left records info
   status = attrCat->getRelInfo(leftRelName, leftAttrCnt, leftAttrDesc);
   if (status != OK){
      delete heapFile;
      heapFile= NULL;
      return status;
   }

   // now gimmie dat right records info
   status = attrCat->getRelInfo(rightRelName, rightAttrCnt, rightAttrDesc);
   if (status != OK){
      delete heapFile;
      heapFile= NULL;
      return status;
   }

   // gotta get dat length
   int i;
   for ( i = 0 ; i < leftAttrCnt ; ++i ){
      leftAttrLen += leftAttrDesc[i].attrLen;
   }
   for ( i = 0 ; i < rightAttrCnt ; ++i ){
      rightAttrLen += rightAttrDesc[i].attrLen;
   }

   int numUnpinnedPages = bufMgr->numUnpinnedPages();
   // number k, that tells you how many pages in the buffer pool you can use for the sort

   if (status != OK){
      delete heapFile;
      heapFile= NULL;
      return status;
   }

   // calculate how many tuples, n, would be contained in k pages.
   double numSortPages = 0.80*(numUnpinnedPages);
   int leftAttrMaxItems = numSortPages * PAGESIZE / leftAttrLen;
   int rightAttrMaxItems = numSortPages * PAGESIZE / rightAttrLen;

   // sort file for left attribute
   SortedFile* leftAttrSortedFile = new SortedFile(leftRelName,
                                                   attrDesc1.attrOffset,
                                                   attrDesc1.attrLen,
                                                   attrType1,
                                                   leftAttrMaxItems,
                                                   status);
   if (status != OK){
      delete heapFile;
      heapFile= NULL;
      delete leftAttrSortedFile;
      leftAttrSortedFile = NULL;
      return status;
   }

   // sort file for right attribute
   SortedFile* rightAttrSortedFile = new SortedFile(rightRelName,
                                                   attrDesc2.attrOffset,
                                                   attrDesc2.attrLen,
                                                   attrType2,
                                                   rightAttrMaxItems,
                                                   status);
   if (status != OK){
      delete heapFile;
      heapFile= NULL;
      delete leftAttrSortedFile;
      leftAttrSortedFile = NULL;
      delete rightAttrSortedFile;
      rightAttrSortedFile = NULL;
      return status;
   }


   Record leftRecord, rightRecord, resultRecord;
   RID leftRID, rightRID, resultRID;

   // save a little room for the data
   // won't you save a little
   // save a little for meeeeeeeee ohhhh
   resultRecord.data = malloc(reclen);

   Status leftStatus = leftAttrSortedFile->next(leftRecord);

   if (leftStatus != OK){
      delete heapFile;
      heapFile= NULL;
      delete leftAttrSortedFile;
      leftAttrSortedFile = NULL;
      delete rightAttrSortedFile;
      rightAttrSortedFile = NULL;
      return leftStatus;
   }

   Status rightStatus = rightAttrSortedFile->next(rightRecord);

   if (rightStatus != OK){
      delete heapFile;
      heapFile= NULL;
      delete leftAttrSortedFile;
      leftAttrSortedFile = NULL;
      delete rightAttrSortedFile;
      rightAttrSortedFile = NULL;
      return rightStatus;
   }

   MatchRecType recCompare;
   bool markSet = false;
   int resultRecOffset = 0;
   while(true){

      recCompare = matchRecCompare(leftRecord, rightRecord, attrDesc1, attrDesc2);

      switch(recCompare){
         case LEFTLTRIGHT: // left is smaller, increment left
            leftStatus = leftAttrSortedFile->next(leftRecord);
            if (leftStatus == FILEEOF){
               break;
            }
            if (leftStatus != OK){ // any issues?
               delete heapFile;
               heapFile= NULL;
               delete leftAttrSortedFile;
               leftAttrSortedFile = NULL;
               delete rightAttrSortedFile;
               rightAttrSortedFile = NULL;
               return leftStatus;
            }
            // now we check if there's a mark and go back to it
            if(markSet){ // we set our mark on the right file
               // now take that mark off so we don't forget!
               markSet = false;
               // let's go back to the mark
               rightStatus = rightAttrSortedFile->gotoMark();
               if (rightStatus != OK){ // any issues?
                  delete heapFile;
                  heapFile= NULL;
                  delete leftAttrSortedFile;
                  leftAttrSortedFile = NULL;
                  delete rightAttrSortedFile;
                  rightAttrSortedFile = NULL;
                  return rightStatus;
               }
            } // end remove mark

            break;
         case RIGHTLTLEFT: // right is smaller, increment right
            rightStatus = rightAttrSortedFile->next(rightRecord);
            if (rightStatus == FILEEOF){
               break;
            }
            if (rightStatus != OK){ // any issues?
               delete heapFile;
               heapFile= NULL;
               delete leftAttrSortedFile;
               leftAttrSortedFile = NULL;
               delete rightAttrSortedFile;
               rightAttrSortedFile = NULL;
               return rightStatus;
            }
            break;
         case EQUAL:
            // AND IT'S LEGIT! insert record!
            resultRecord.data = malloc(reclen); // allocate enough room for all our shtuff

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

            status = heapFile->insertRecord(resultRecord, resultRID);

            if (status != OK){
               delete heapFile;
               heapFile= NULL;
               delete leftAttrSortedFile;
               leftAttrSortedFile = NULL;
               delete rightAttrSortedFile;
               rightAttrSortedFile = NULL;
               free(resultRecord.data);
               return status;
            }

            if (!markSet){
               // mark our spot in rightFile
               rightStatus = rightAttrSortedFile->setMark();
               if (rightStatus != OK){
                  delete heapFile;
                  heapFile= NULL;
                  delete leftAttrSortedFile;
                  leftAttrSortedFile = NULL;
                  delete rightAttrSortedFile;
                  rightAttrSortedFile = NULL;
                  free(resultRecord.data);
                  return rightStatus;
               }
               // let's look through that
               rightStatus = rightAttrSortedFile->next(rightRecord);
               if (rightStatus == FILEEOF){
                  break;
               }
               if (rightStatus != OK){
                  delete heapFile;
                  heapFile= NULL;
                  delete leftAttrSortedFile;
                  leftAttrSortedFile = NULL;
                  delete rightAttrSortedFile;
                  rightAttrSortedFile = NULL;
                  free(resultRecord.data);
                  return rightStatus;
               }

               markSet = true;
            }
            free(resultRecord.data);
            break;
         default: // matchRec issue?
            delete heapFile;
            heapFile= NULL;
            delete leftAttrSortedFile;
            leftAttrSortedFile = NULL;
            delete rightAttrSortedFile;
            rightAttrSortedFile = NULL;
            free(resultRecord.data);
            return ATTRTYPEMISMATCH;
            break;
      }


   }

   delete heapFile;
   heapFile= NULL;
   delete leftAttrSortedFile;
   leftAttrSortedFile = NULL;
   delete rightAttrSortedFile;
   rightAttrSortedFile = NULL;
   return OK;
}
