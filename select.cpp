#include "catalog.h"
#include "query.h"
#include "index.h"
#include "string.h"

/*
* Selects records from the specified relation.
*
* Returns:
OK on success
an error code otherwise
*/
Status Operators::Select(const string & result,      // name of the output relation
                        const int projCnt,          // number of attributes in the projection
                        const attrInfo projNames[], // the list of projection attributes
                        const attrInfo *attr,       // attribute used inthe selection predicate
                        const Operator op,         // predicate operation
                        const void *attrValue)     // literal value in the predicate
{
   AttrDesc whereAttrDesc;
   int resultAttrsLength = 0;
   Status status;
   AttrDesc* resultAttrDesc = new AttrDesc[projCnt];

   // TODO: write checks to make sure that attr->attrType == the type of the contents of attrValue

   for(int i = 0; i < projCnt; i++){

      status = attrCat->getInfo(projNames[i].relName, projNames[i].attrName, resultAttrDesc[i]);

      if(status != OK){
         delete[] resultAttrDesc;
         return status;
      }

      resultAttrsLength += resultAttrDesc[i].attrLen;
   }

   // if attr is null, this means that the selection is unconditional
   // in people words, there's no WHERE clause
   if(attr != NULL){
      string selectAttrRelName = attr->relName;
      string selectAttrName = attr->attrName;
      status = attrCat->getInfo(selectAttrRelName, selectAttrName, whereAttrDesc);
      if(status != OK){
         delete[] resultAttrDesc;
         return status;
      }

      // if it's not an equality operation -> use scan select
      // ----  OR  ----
      // if it's not indexed -> use scan select

      if(op != EQ || !whereAttrDesc.indexed){
         status = ScanSelect(result, projCnt, resultAttrDesc, &whereAttrDesc, op, attrValue, resultAttrsLength);
      }
      else{
         status = IndexSelect(result, projCnt, resultAttrDesc, &whereAttrDesc, op, attrValue, resultAttrsLength);
      }
   }
   else{ // there's no where clause doe, so we gotta scan
      AttrDesc* emptyAttrDesc = NULL; // still gotta pass something empty in
      attrValue = NULL; // just in case
      status = ScanSelect(result, projCnt, resultAttrDesc, emptyAttrDesc, op, attrValue, resultAttrsLength);
   }

   if(status != OK){
      delete[] resultAttrDesc;
      return status;
   }

   delete[] resultAttrDesc;
   return OK;
}
