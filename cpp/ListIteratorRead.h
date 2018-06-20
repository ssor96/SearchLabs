#include "ListIterator.h"
#include "Reader.h"
#include "JumpTable.h"

class ListIteratorRead : ListIterator {
private:
    DiffReader reader;
    JumpTable jumpTable;
    int pos;
public:
    int sz;
    ListIteratorRead(uchar*, uchar*, int);
    void next();
    ~ListIteratorRead(){}
};