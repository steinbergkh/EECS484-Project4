#include "catalog.h"
#include "query.h"
#include "index.h"

/*
 * A simple scan select using a heap file scan
 */

Status Operators::ScanSelect(const string& result,       // Name of the output relation
                             const int projCnt,          // Number of attributes in the projection
                             const AttrDesc projNames[], // Projection list (as AttrDesc)
                             const AttrDesc* attrDesc,   // Attribute in the selection predicate
                             const Operator op,          // Predicate operator
                             const void* attrValue,      // Pointer to the literal value in the predicate
                             const int reclen)           // Length of a tuple in the result relation
{
  cout << "Algorithm: File Scan" << endl;

  /* Your solution goes here */
  Status status;
  HeapFile(result, status);

  if (status != OK){
     return status;
  }

  HeapFileScan *heapFileScan;
  if(attrDesc){ //where clause exists -> gotta filter
    Datatype projAttrType = (Datatype)attrDesc->attrType;
    char *projAttrVal = (char*)attrValue;  //do we have to static cast this?
    heapFileScan = new HeapFileScan(attrDesc->relName, attrDesc->attrOffset, attrDesc->attrLen,
                                    projAttrType, projAttrVal, op, status);
  }
  else { //no where clause
    heapFileScan = new HeapFileScan(projNames[0].relName, status);
    heapFileScan->startScan(-1, //if offset < 0  return bad scan param error
                            0, //if length < 1  return bad scan param error
                            (Datatype)0, //do we have to static cast this?
                            NULL, //no filtering requested, should make function return okay
                            (Operator)0);
  }

  if (status != OK){
     return status;
  }

  // now we gotta get matching records from our
  // heapFileScans and put them in our result relation

  return OK;
}
