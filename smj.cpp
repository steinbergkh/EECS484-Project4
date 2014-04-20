#include "catalog.h"
#include "query.h"
#include "sort.h"
#include "index.h"

/* Consider using Operators::matchRec() defined in join.cpp
 * to compare records when joining the relations */

Status Operators::SMJ(const string& result,           // Output relation name
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

  int numUnpinnedPages = bufMgr->numUnpinnedPages();
  // number k, that tells you how many pages in the buffer pool you can use for the sort
  double numSortPages = 0.80*(numUnpinnedPages);

  if (status != OK){
     delete heapFile;
     heapFile= NULL;
     return status;
  }

  // consult the system catalogs and determine the size of the tuples (in # of bytes)
  RelDesc relDesc1;
  RelDesc relDesc2;

  // calculate how many tuples, n, would be contained in k pages.

  return OK;
}
