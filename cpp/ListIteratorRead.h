#include "ListIterator.h"
#include "Reader.h"
#include "JumpTable.h"

class ListIteratorRead : public ListIterator {
private:
    DiffReader reader;
    JumpTable jumpTable;
    int pos;
public:
    int sz;
    ListIteratorRead(uchar* readerData, uchar* jumpData, int sz);
    void next();
    bool jump(int);
    ~ListIteratorRead(){}
};