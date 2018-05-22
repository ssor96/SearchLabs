#include "ListIterator.h"

class ListIteratorRead : ListIterator {
private:

public:
    int sz;
    int pos;
    T *elements;
    ListIteratorRead(T *elements, int sz): elements(elements), sz(sz) {
        pos = 0;
    }
    void next();
    ~ListIteratorRead(){}
};