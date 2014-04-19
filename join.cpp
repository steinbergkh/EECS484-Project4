#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "index.h"
#include <cmath>
#include <cstring>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define DOUBLEERROR 1e-07

/*
* Joins two relations
*
* Returns:
OK on success
an error code otherwise
*/

Status Operators::Join(const string& result,        // Name of the output relation
                        const int projCnt,          // Number of attributes in the projection
                        const attrInfo projNames[], // List of projection attributes
                        const attrInfo* attr1,      // Left attr in the join predicate
                        const Operator op,          // Predicate operator
                        const attrInfo* attr2)      // Right attr in the join predicate
{
   /* Your solution goes here */
   Status status;

   AttrDesc* projAttrDesc = new AttrDesc[projCnt];
   AttrDesc leftAttrDesc, rightAttrDesc;
   int recordLength = 0;

   int i;
   for (i = 0; i < projCnt ; ++i){
      status = attrCat->getInfo(projNames[i].relName,
                                 projNames[i].attrName,
                                 projAttrDesc[i]);
      if (status != OK){
         delete [] projAttrDesc;
         return status;
      }
      recordLength += projAttrDesc[i].attrLen;
   }
   status = attrCat->getInfo(attr1->relName,
                              attr1->attrName,
                              leftAttrDesc);
   if (status != OK){
      delete [] projAttrDesc;
      return status;
   }

   status = attrCat->getInfo(attr2->relName,
                              attr2->attrName,
                              rightAttrDesc);
   if (status != OK){
      delete [] projAttrDesc;
      return status;
   }

   if (op == EQ){ // if equal, must check for index on either attribute

      // INL says it evaluates joins with an index on the inner/right relation
      // so if it's an index on the outer (left) relation, we switch the two
      // cuz we $m@r+.

      if (rightAttrDesc.indexed){
         // there's an index on attr2, so we can use INDEX NESTED LOOP JOIN

         status = INL(result, projCnt, projAttrDesc, leftAttrDesc, op, rightAttrDesc, recordLength);
      }
      else if(leftAttrDesc.indexed){
         // there's an index on attr1, so we can use INDEX NESTED LOOP JOIN

         status = INL(result, projCnt, projAttrDesc, rightAttrDesc, op, leftAttrDesc, recordLength);
      }
      else{
         // no index, but it is an equi-join, so we use SORT MERGE JOIN

         status = SMJ(result, projCnt, projAttrDesc, leftAttrDesc, op, rightAttrDesc, recordLength);
      }
   }
   else{ // gotta use SIMPLE NESTED LOOP JOIN
      status = SNL(result, projCnt, projAttrDesc, leftAttrDesc, op, rightAttrDesc, recordLength);
   }

   if (status != OK){
      delete [] projAttrDesc;
      return status;
   }

   return OK;
}

// Function to compare two record based on the predicate. Returns 0 if the two attributes
// are equal, a negative number if the left (attrDesc1) attribute is less that the right
// attribute, otherwise this function returns a positive number.
int Operators::matchRec(const Record& outerRec,    // Left record
                        const Record& innerRec,    // Right record
                        const AttrDesc& attrDesc1, // Left attribute in the predicate
                        const AttrDesc& attrDesc2) // Right attribute in the predicate
{
   int tmpInt1, tmpInt2;
   double tmpFloat1, tmpFloat2, floatDiff;

   // Compare the attribute values using memcpy to avoid byte alignment issues
   switch(attrDesc1.attrType)
   {
      case INTEGER:
      memcpy(&tmpInt1, (char *) outerRec.data + attrDesc1.attrOffset, sizeof(int));
      memcpy(&tmpInt2, (char *) innerRec.data + attrDesc2.attrOffset, sizeof(int));
      return tmpInt1 - tmpInt2;

      case DOUBLE:
      memcpy(&tmpFloat1, (char *) outerRec.data + attrDesc1.attrOffset, sizeof(double));
      memcpy(&tmpFloat2, (char *) innerRec.data + attrDesc2.attrOffset, sizeof(double));
      floatDiff = tmpFloat1 - tmpFloat2;
      return (fabs(floatDiff) < DOUBLEERROR) ? 0 : (floatDiff < 0?floor(floatDiff):ceil(floatDiff));

      case STRING:
      return strncmp(
      (char *) outerRec.data + attrDesc1.attrOffset,
      (char *) innerRec.data + attrDesc2.attrOffset,
      MAX(attrDesc1.attrLen, attrDesc2.attrLen));
   }

   return 0;
}

bool Operators::streq( const char * str1, const char * str2 ){
   int retVal = strcmp(str1, str2);
   if (retVal == 0){
      return true;
   }
   else{
      return false;
   }
}
