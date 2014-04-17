#include "catalog.h"
#include "query.h"
#include "index.h"
Status Operators::IndexSelect(const string& result,       // Name of the output relation
                              const int projCnt,          // Number of attributes in the projection
                              const AttrDesc projNames[], // Projection list (as AttrDesc)
                              const AttrDesc* attrDesc,   // Attribute in the selection predicate
                              const Operator op,          // Predicate operator
                              const void* attrValue,      // Pointer to the literal value in the predicate
                              const int reclen)           // Length of a tuple in the output relation
{
  cout << "Algorithm: Index Select" << endl;

  /* Your solution goes here */
  Datatype indexDatatype = Datatype(attrDesc->attrType); // static cast attrType
                                                         // from int to Datatype
  string indexRelName = attrDesc->relName;
  Status status;
  Index *attrIndex = new Index(indexRelName,    // name of the relation being indexed
                        attrDesc->attrOffset,   // offset of the attribute being indexed
                        attrDesc->attrLen,      // length of the attribute being indexed
                        indexDatatype,          // type of the attribute being indexed
                        0,                      // =1 if the index should only allow unique entries.
                        status);        // return error codes

   // this func is only called if there is an index on the relation "indexRelName"
   // which means the constructor should find the file and put the header page
   // into the buffer poo



  return OK;
}
