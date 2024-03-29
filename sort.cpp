#include <sstream>
#include <string.h>
#include <stdlib.h>
#include "sort.h"

#define MIN(a,b)   ((a) < (b) ? (a) : (b))


// These comparison functions are visible only within this
// source file. reccmp is the comparison routine (much like
// strcmp or memcmp) that accepts integers, floats, and strings.
// It returns -1 if p1 is less than p2, +1 if p1 is greater
// than p2, or zero otherwise.

static int reccmp(char* p1, char* p2, int p1Len, int p2Len, Datatype type)
{
  float diff = 0.0;

  switch(type) {
  case INTEGER:
    int iattr, ifltr;                   // word-alignment problem possible
    memcpy(&iattr, p1, sizeof(int));
    memcpy(&ifltr, p2, sizeof(int));
    diff = iattr - ifltr;
    break;
  case DOUBLE:
    double fattr, ffltr;                 // word-alignment problem possible
    memcpy(&fattr, p1, sizeof(double));
    memcpy(&ffltr, p2, sizeof(double));
    diff = fattr - ffltr;
    break;
  case STRING:
    diff = memcmp(p1, p2, MIN(p1Len, p2Len));
    break;
  default:
    break;
  }

  if (diff < 0)
    diff = -1;
  else if (diff > 0)
    diff = 1;

  return (int)diff;
}


// These three comparison routines are jacketed versions of
// reccmp. This is because qsort(3) takes only a function pointer
// but no additional parameters. The objects pointed to by p1
// and p2 are of type SORTREC which has a pointer to the field
// to be compared as well as its length (used for strings).

#define SR(p)  ((SORTREC*)p)

static int intcmp(const void* p1, const void* p2)
{
 p1 = (char*) p1;
 p2 = (char*) p2;
  return reccmp(SR(p1)->field, SR(p2)->field,
		SR(p1)->length, SR(p2)->length,
		INTEGER);
}


static int floatcmp(const void* p1, const void* p2)
{
 p1 = (char*) p1;
 p2 = (char*) p2;
  return reccmp(SR(p1)->field, SR(p2)->field,
		SR(p1)->length, SR(p2)->length,
		DOUBLE);
}


static int stringcmp(const void* p1, const void* p2)
{
 p1 = (char*) p1;
 p2 = (char*) p2;
  return reccmp(SR(p1)->field, SR(p2)->field,
		SR(p1)->length, SR(p2)->length,
		STRING);
}


// Create a sorted temporary file of the source file (fileName).
// Sorting is based on attribute that is defined by offset, len,
// and type. maxItems is the maximum number of items that a sorted
// sub-run can hold (usually derived from amount of memory available).
// Status code is returned in variable status.

SortedFile::SortedFile(const string & fileName,
		       int offset, int len, Datatype type,
		       int maxItems, Status& status)
      : fileName(fileName), type(type), offset(offset),
	length(len), maxItems(maxItems)
{
  // Check incoming parameters.

  status = OK;

  if (offset < 0 || len < 1)
    status = BADSORTPARM;
  else if (type != STRING && type != INTEGER && type != DOUBLE)
    status = BADSORTPARM;
  else if ((type == INTEGER && len != sizeof(int))
	   || (type == DOUBLE && len != sizeof(double)))
    status = BADSORTPARM;

  if (status != OK)
    return;

  // Must have space for at least 2 items (records) because otherwise
  // items cannot be swapped and sorted!

  if (maxItems < 2 || !(buffer = new SORTREC [maxItems])) {
    status = INSUFMEM;
    return;
  }

  status = sortFile();
}


// Sort file into sub-runs. The source file is split into runs
// which have at most maxItems records each. That many records
// are read into memory, sorted using qsort(3), and then written
// to a temporary file.

Status SortedFile::sortFile()
{
  Status status;
  Record rec;

  // Open source file.

  // Start an unfiltered sequential scan.

  if (!(file = new HeapFileScan(fileName, 0, 0, STRING, NULL, EQ, status)))
    return INSUFMEM;

  if (status != OK)
    return status;

  // As long as the source file has more records, collect up to
  // maxItems records into buffer and then dump records into
  // temporary file.

  do {
    for(numItems = 0; numItems < maxItems; numItems++) {

      // Fetch next record from source file, check if end of file.

      if ((status = file->scanNext(buffer[numItems].rid)) == FILEEOF)
	break;
      else if (status != OK)
	return status;
      if ((status = file->getRecord(buffer[numItems].rid,
				    rec)) != OK)
	return status;

      // Create space for holding a copy of the sorting attribute
      // only (rest of record is read when temporary file is
      // written). Copy sorting attribute from source record and
      // store the length of the attribute (reccmp is general-
      // purpose and can be shared by multiple instances of
      // SortedFile!).

      if (!(buffer[numItems].field = new char [length]))
	return INSUFMEM;
      memcpy(buffer[numItems].field, (char *)rec.data + offset, length);
      buffer[numItems].length = length;
    }

    // If at least 1 record in sub-run, sort records and write out
    // to temporary file.

    if (numItems > 0) {
      if ((status = generateRun(numItems)) != OK)
	return status;
      for(int i = 0; i < numItems; i++)
	delete [] buffer[i].field;
    }
  } while (numItems > 0);

  // Terminate sequential scan on source file and close file.

  delete file;

  // Prepare a sequential scan on each sub-run so that next()
  // can fetch next record from each run.

  if ((status = startScans()) != OK)
    return status;

  return OK;
}


// Sort the records in buffer[] (actually, the sorting attribute
// plus the associated RID) and then dump records into temporary
// file.

Status SortedFile::generateRun(int items)
{
  Status status;

  // Sort buffer using library function qsort (quick sort). Use
  // the appropriate comparison function for integers, floats,
  // or strings (qsort can't take type as a parameter).

  if (type == INTEGER)
    qsort(buffer, items, sizeof(SORTREC), intcmp);
  else if (type == DOUBLE)
    qsort(buffer, items, sizeof(SORTREC), floatcmp);
  else
    qsort(buffer, items, sizeof(SORTREC), stringcmp);

  // If this is the first sub-run, malloc space for a RUN object,
  // otherwise realloc more space. Note that on most systems
  // realloc(NULL) could be used even when runs == NULL, but
  // this doesn't work on all systems.

  RUN newRun;
  runs.push_back(newRun);

  // If failed to create space for an additional run.

   RUN & run = runs.back();

  // Generate file name for temporary file.

  ostringstream outputString;
  outputString << fileName << ".sort." << runs.size() << ends;
  run.name = outputString.str();
  /*
  char *tmpString = outputString.str();
  run.name = string(tmpString);
  delete [] tmpString;
  */


#ifdef DEBUGSORT
  cout << "%%  Writing " << items << " tuples to file " << run->name
       << endl;
#endif

  // Make sure temporary file does not exist already. We don't
  // want to corrupt somebody else's sorted files (on another
  // attribute, for example).

  if ((status = db.createFile(run.name)) != OK)
    return status;                      // file must not exist already
  if ((status = db.destroyFile(run.name)) != OK)
    return status;                      // delete if successful

  // Open a heap file. This will also create the temporary file.

  if (!(run.file = new HeapFileScan(run.name, 0, 0, STRING, NULL, EQ, status)))
    return INSUFMEM;

  if (status != OK)
    return status;

  // Open an unfiltered sequential scan on the source file. The scan
  // is not actually needed for anything else than just getting
  // a scanId which getRandomRecord requires.

  HeapFileScan scan (fileName, 0, 0, STRING, NULL, EQ, status);

  if (status != OK)
    return status;

  // For each sort record (attribute plus RID) in the buffer, fetch
  // the whole record from the source file and then insert it into
  // the temporary file.

  for(int i = 0; i < items; i++) {
    SORTREC* rec = &buffer[i];
    RID rid;
    Record record;

    if ((status = scan.getRandomRecord(rec->rid, record)) != OK)
      return status;

    if ((status = run.file->insertRecord(record, rid)) != OK)
      return status;
  }

  delete run.file;

  return OK;
}


// Prepare a sequential scan on each sub-run so that next()
// can fetch the next record from each run. The valid bit of
// each run is marked false to indicate that the (first)
// record has not been fetched yet. next() must therefore
// fetch it.

Status SortedFile::startScans()
{
  Status status;
  vector<RUN>::iterator run;

#ifdef DEBUGSORT
  cout << "Start scan on : ";
#endif
  for(run = runs.begin(); run != runs.end(); run++)
    {
      run->file = new HeapFileScan(run->name, 0, 0, STRING, NULL, EQ, status);
#ifdef DEBUGSORT
      cout << run->name << ", " << endl;
#endif

      if (status != OK)
	return status;
      run->valid = false;
      run->rid.pageNo = -1;
      run->rid.slotNo = -1;
      run->mark.pageNo = -1;
      run->mark.slotNo = -1;
    }
#ifdef DEBUGSORT
    cout << endl;
#endif

  return OK;
}


// Retrieve the next smallest record from the set of sorted sub-runs.
// The next record of each sub-run is peeked to find out the
// smallest of all. The pointer in the chosen sub-run is then
// advanced.

Status SortedFile::next(Record & rec)
{
  Status status;

  // Empty source file has zero sub-runs and causes
  // end of file to be returned.

  if (runs.size() <= 0)
    return FILEEOF;

  // Find the run which has the smallest next record. If a run
  // has false valid bit, it doesn't have the next record in memory
  // yet.

  // RUN* smallest = NULL;
  bool foundSmallest = false;
  vector<RUN>::iterator run, smallest;

  for(run = runs.begin(); run != runs.end(); run++)
    {
      if (run->valid == false) {          // no record fetched yet for this run?
	status = run->file->scanNext(run->rid);
	if (status == FILEEOF)            // reached end of this run file?
	  run->rid.pageNo = -1;           // mark end of file
	else if (status != OK)
	  return status;
	else {                            // if next record exists, fetch it
	  if ((status = run->file->getRecord(run->rid, run->rec)) != OK)
	    return status;
	}
	run->valid = true;                // a record is now in memory
      }

      if (run->rid.pageNo < 0)            // end of run already?
	continue;

      if (!foundSmallest)                 // select first one as smallest
      {
	smallest = run;
        foundSmallest = true;
      }
      else if (reccmp((char *)smallest->rec.data + offset,
		      (char *)run->rec.data + offset,
		      length, length, type) > 0)
      {
	smallest = run;                   // current run had smaller next
        foundSmallest = true;
      }
    }

  if (!foundSmallest)                     // no next record found?
    return FILEEOF;

#ifdef DEBUGSORT
  cout << "%%  Retrieved smallest from " << smallest->name << endl;
#endif

  rec = smallest->rec;                    // give record pointers to caller

  smallest->valid = false;                // must fetch new record next time

  return OK;
}


// Remember a position in the sorted output so that the caller
// can later return to this spot. The current RID of each sub-run
// is recorded in a separate field.

Status SortedFile::setMark()
{
#ifdef DEBUGSORT
  cout << "%%  Setting mark in file" << endl;
#endif

  vector<RUN>::iterator run;

  for(run = runs.begin(); run != runs.end(); run++)
    {
      run->mark.pageNo = run->rid.pageNo;
      run->mark.slotNo = run->rid.slotNo;
#ifdef DEBUGSORT
      cout << "%%  Run " << i << " is at page " << run->mark.pageNo
	   << ", slot " << run->mark.slotNo << endl;
#endif
    }

  return OK;
}


// Restore sort position by fetching the last marked record
// of each sub-run using getRandomRecord. This allows the caller
// to back up in the sorted sequence (used in sort-merge join
// in case of duplicates).

Status SortedFile::gotoMark()
{
#ifdef DEBUGSORT
  cout << "%%  Going to a mark in file" << endl;
#endif

  Status status;
  vector<RUN>::iterator run;

  for(run = runs.begin(); run != runs.end(); run++)
    {
      run->rid.pageNo = run->mark.pageNo;
      run->rid.slotNo = run->mark.slotNo;

#ifdef DEBUGSORT
      if (run->rid.pageNo >= 0) {
	cout << "%%  Run " << i << " going to page " << run->rid.pageNo
	     << ", slot " << run->rid.slotNo << endl;
      } else {
	cout << "%%  Run " << i << " is already at eof" << endl;
      }
#endif

      // Restore file position only if last marked position is
      // something else than end of file.

      if (run->rid.pageNo >= 0) {
	if ((status = run->file->getRandomRecord(run->rid, run->rec)) != OK)
	  return status;
      }

      // Current record is already in memory so next() must not
      // advance in the temporary file.

      run->valid = true;
    }

  return OK;
}


// Deallocate all space allocated for this sorted file and
// delete temporary files.

SortedFile::~SortedFile()
{
  for(unsigned int i = 0; i < runs.size(); i++) {
    delete runs[i].file;
    (void)db.destroyFile(runs[i].name);
  }

  delete [] buffer;
}
