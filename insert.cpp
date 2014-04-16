#include "catalog.h"
#include "query.h"
#include "index.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace std;

bool strEqual( const char * str1, const char * str2 ){
   if (!strcmp(str1, str2)){ // strcmp returns 0 when strings are equal
      return true;
   }
   else{
      return false;
   }
}

/*
* Inserts a record into the specified relation
*
* Returns:
* 	OK on success
* 	an error code otherwise
*/

Status Updates::Insert(const string& relation,      // Name of the relation
const int attrCnt,           // Number of attributes specified in INSERT statement
const attrInfo attrList[])   // Value of attributes specified in INSERT statement
{
   /* Your solution goes here */
   Status status;
   AttrDesc *attrs;
   int recLength = 0;

   int attributeCountTrue = 0;

   // get attribute data
   status = attrCat->getRelInfo(relation, attributeCountTrue, attrs);

   // status check of getRelInfo
   if (status != OK){
      return status;
   }

   // check if number of attributes is right
   // (true if either missing some or has duplicates)
   if (attributeCountTrue != attrCnt ){
      cout << "attributeCountTrue == "<< attributeCountTrue << "attrCnt == " << attrCnt << endl;
      if (attributeCountTrue > attrCnt){ // missing some
         status = ATTRTYPEMISMATCH;
      }
      else{ // duplicate
         cout << "attribute count true is less than attribute counts entered" << endl;
         status = DUPLATTR;
      }
   }

   // status check of getRelInfo
   if (status != OK){
      return status;
   }


   // TODO if no value is specified for an attribute in attrList, you should reject the insertion
   // status = ATTRNOTFOUND; //????

   Record record;
   int i;

      // get the record size
      for(i = 0; i < attrCnt; i++) {
         recLength += attrs[i].attrLen;
      }
      record.data = malloc(recLength); // REMEMBER TO FREE!
      record.length = recLength;

      // set to true at index where attribute was found
      // and value was inserted into record (detects
      // duplicate attr/extra attrs)
      bool dataInserted[attrCnt];
      bool attrFound[attrCnt];
      for (int i = 0; i < attrCnt ; ++i){
         dataInserted[i] = false;
         attrFound[i] = false;
      }


      int i_attribute, i_insert;
      void* recValAddr = (void *)record.data;
      for (i_attribute = 0 ; i_attribute < attrCnt ; ++i_attribute){
         for (i_insert = 0 ; i_insert < attrCnt ; ++i_insert){
            if(!dataInserted[i_insert]
            && strEqual( attrs[i_attribute].attrName, attrList[i_insert].attrName)){
               // data hasn't been inserted yet & attribute strings are equal
               if (attrs[i_attribute].attrType != attrList[i_insert].attrType){
                  status = ATTRTYPEMISMATCH;
                  break;
               }
               else if (attrFound[i_attribute]){
                  // attribute was already matched with other attrInfo
                  cout << "attribute was already matched with other attrInfo" << endl;
                  status = DUPLATTR;
                  break;
               }
               else if (attrs[i_attribute].attrLen < attrList[i_insert].attrLen){
                  status = ATTRTOOLONG;
                  break;
               }
               else{
                  attrFound[i_attribute] = true;
                  dataInserted[i_insert] = true;

                  // fill in data with values from INSERT
                  recValAddr += attrs[i_attribute].attrOffset;
                  memcpy(recValAddr, attrList[i_insert].attrValue, attrs[i_attribute].attrLen);
               }
            }
         }
         // if bad things happened, we free the data previously allocated
         // for the record and return the status error
         if (status != OK){
            free(record.data);
            return status;
         }

      }

      // INSERT THE RECORD INTO THE HEAPFILE
      HeapFile *heapFile = new HeapFile(relation, status);

      // if error, delete allocated HeapFile and return the error status
      if(status != OK){
         delete heapFile;
         return status;
      }
      RID recordID;
      // insert record
      status = heapFile->insertRecord(record, recordID);
      if(status != OK){
         delete heapFile;
         return status;
      }
      delete heapFile;

      // RECORD ID NEEDS TO BE INSERTED IN EACH INDEX FOR THE RELATION
      // 1. check catalogs to see if there is one or more indexes for the relation
      for(i = 0; i < attrCnt; i++) {
         if(attrs[i].indexed){ // there is an index on the attribute

            Datatype attrDatatype = (Datatype)attrs[i].attrType; // cast to ENUM Datatype
            Index *attrIndex = new Index(relation, attrs[i].attrOffset,
                                       attrs[i].attrLen, attrDatatype, 0, status);

            // check to make sure nothin went wrong
            if(status != OK){
               delete attrIndex;
               return status;
            }

            attrIndex->insertEntry(record.data, recordID);

            // check to make sure nothin went wrong
            if(status != OK){
               delete attrIndex;
               return status;
            }

            // free up dat mem
            delete attrIndex;
         }
      }


      return status;
   }
