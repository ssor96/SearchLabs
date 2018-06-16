#include "ListIterator.h"
#include "Reader.h"

class ListIteratorRead : ListIterator {
private:

public:
    int sz;
    int pos;
    unsigned char *elements;
    DiffReader reader;
    ListIteratorRead(unsigned char *elements, int sz): elements(elements), sz(sz) {
        pos = 0;
        reader.data = elements;
    }
    void next();
    ~ListIteratorRead(){}
};