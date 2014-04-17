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
  Heapfile(result, status);

  HeapFileScan *heapFileScan;
  if(attrDesc){ //where clause exists -> gotta filter
    Datatype projAttrType = (Datatype)attrDesc->attrType;
    char *projAttrVal = (char*)attrValue;  //do we have to static cast this?
    heapFileScan = new HeapFileScan(attrDesc->relname, attrDesc->attrOffset, attrDesc->attrLen,
                                    projAttrType, projAttrVal, op, status);
  }
  else { //no where clause
    heapFileScan = new HeapFileScan(projNames[0].relname, status);
    heapFileScan->startScan(-1, //if offset < 0  return bad scan param error
                            0, //if length < 1  return bad scan param error
                            (DataType)0, //do we have to static cast this?
                            NULL, //no filtering requested, should make function return okay
                            (Operator)0);
  }

  // TODO check status here!

  return OK;
}
