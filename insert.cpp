#include "catalog.h"
#include "query.h"
#include "index.h"
#include <stdlib.h>
#include <string.h>

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
    RelDesc rd;
    AttrDesc *attrs = new AttrDesc();
    int recLength = 0;

    int attributeCountTrue = attrCnt;

    // get attribute data
    status = attrCat->getRelInfo(rd.relName, attributeCountTrue, attrs);

   // check if number of attributes is right
   // (true if either missing some or has duplicates)
   if (attributeCountTrue != attrCnt ){
      if (attributeCountTrue > attrCnt){ // missing some
         status = ATTRTYPEMISMATCH;      }
      else{ // duplicate
         status = DUPLATTR;
      }
   }

   // status check of getRelInfo
   if (status != OK){
     return status;
  }


   // TODO if no value is specified for an attribute in attrList, you should reject the insertion
   // status = ATTRNOTFOUND; //????

   Record rec;
   int i;

   // get the record size
   for(i = 0; i < attrCnt; i++) {
     recLength += attrs[i].attrLen;
   }
   rec.data = malloc(recLength);
   rec.length = recLength;

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
   void* recValAddr = (void *)rec.data;
   for (i_attribute = 0 ; i_attribute < attrCnt ; ++i_attribute){
      for (i_insert = 0 ; i_insert < attrCnt ; ++i_insert){
         if(!dataInserted[i_insert]
            && strEqual( attrs[i_attribute].attrName, attrList[i_insert].attrName)){
               // data hasn't been inserted yet & attribute strings are equal
               if (attrs[i_attribute].attrType != attrList[i_insert].attrType){
                  status = ATTRTYPEMISMATCH;
                  return status;
               }
               else if (attrFound[i_attribute]){
                  // attribute was already matched with other attrInfo
                  status = DUPLATTR;
                  return status;
               }
               attrFound[i_attribute] = true;
               dataInserted[i_insert] = true;

         }
      }
   }

    return status;
}

bool strEqual( const char * str1, const char * str2 ){
   if (!strcmp(str1, str2)){ // strcmp returns 0 when strings are equal
      return true;
   }
   else{
      return false;
   }
}
