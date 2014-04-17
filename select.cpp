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
   AttrDesc * whereAttrDesc;
   RelDesc outputRelationDesc;
   int recordLength = 0;
   Status status;
   AttrDesc * projectedAttrDesc = new AttrDesc[projCnt];

   strcpy(outputRelationDesc.relName, result.c_str());
   outputRelationDesc.attrCnt = projCnt;
   outputRelationDesc.indexCnt = 0;

   for(int i = 0; i < outputRelationDesc.attrCnt; i++){
      string projRelName = projNames[i].relName;
      string projAttrName = projNames[i].attrName;
      status = attrCat->getInfo(projRelName, projAttrName, &projectedAttrDesc[i]);

      if(status != OK){
         delete[] projectedAttrDesc;
         return status;
      }
      projectedAttrDesc[i].indexed = 0;
      recordLength += projectedAttrDesc[i].attrLen;
   }

   // if attr is null, this means that the selection is unconditional
   // in people words, there's no WHERE clause
   if(attr != NULL){
      status = attrCat->getInfo(string(attr->relName), string(attr->attrName), whereAttrDesc);
      if(status != OK){
         delete[] projectedAttrDesc;
         return status;
      }
   }
   else{ // there's no where clause doe
   whereAttrDesc = NULL;
   }

   // if it's not an equality operation -> use scan select
   // ----  OR  ----
   // if it's not indexed -> use scan select
   if(op != EQ || !attrDesc.indexed){
      status = ScanSelect(result, projCnt, projectedAttrDesc, whereAttrDesc, op, attrValue, recordLength);
   }
   else{
      status = IndexSelect(result, projCnt, projectedAttrDesc, whereAttrDesc, op, attrValue, recordLength);
   }
   if(status != OK){
      delete[] projectedAttrDesc;
      return status;
   }

   delete[] projectedAttrDesc;
   return OK;
}
