#EECS 484 -- Project 4
##Sort Merge Join

For implementing sort-merge join, you can use the sorting code provided in files sort.h and sort.cpp
but you will need to implement the merge phase.

- When performing a sort you will use 80% of the unpinned pages for the sort
- Call BufMgr::numUnpinnedPages to determine the # of pages that are unpinned in the buffer pool
- Set the number of pages that you will use for the sort at 80% of this number (performance starts going down because of thrashing if the resource is overcommitted)
- You now have a number, k, that tells you how many pages in the buffer pool you can use for the sort
- Consult the system catalogs and determine the size of the tuple (in number of bytes)
- Since Minirel pages are 1KB (defined by PAGESIZE), you can calculate how many tuples, n, would be contained in k pages

To handle duplicates, you will sometimes need to move the scan backwards
- The SortedFile class has two new functions setMark() which sets a marker at the current tuple being scanned
- Call the setMark() only after you have called next() to get a record.
- A marker will be set on the record that was fetched by the last call to next().
- To go back to scanning from the mark point, use the method gotoMark(), which moves the scan back to the last mark point, and retrieves the record at the marker
- After calling gotoMark(), you can call next() to keep scanning forward from the marker point
