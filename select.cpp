#include "catalog.h"
#include "query.h"
#include "index.h"
#include "string.h"

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */
Status Operators::Select(const string & result,      // name of the output relation
	                 const int projCnt,          // number of attributes in the projection
		         const attrInfo projNames[], // the list of projection attributes
		         const attrInfo *attr,       // attribute used inthe selection predicate
		         const Operator op,         // predicate operation
		         const void *attrValue)     // literal value in the predicate
{
   Status status;

   // initialize result relation
   RelDesc resultRel;
   strcpy(resultRel.relName, result);
   resultRel.attrCnt = projCnt // projected attributes are the
                              // attributes of the result relation
   resultRel.indexCnt = 0; // no indexes on the result relation b/c it's temp
   AttrDesc * resultAttrDesc = new AttrDesc[projCnt];


   return OK;
}
