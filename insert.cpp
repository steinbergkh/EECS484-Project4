#include "catalog.h"
#include "query.h"
#include "index.h"

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

    // get attribute data
    status = attrCat->getRelInfo(rd.relName, attrCnt, attrs);

    // status check of getRelInfo
    if (status != OK){
      return status;
   }

   Record rec;
   int i;

   // get the record size
   for(i = 0; i < attrCnt; i++) {
     recLength += attrs[i].attrLen;
   }
   rec.data = malloc(recLength);
   rec.length = recLength;

   //

    return status;
}
