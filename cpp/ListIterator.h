#ifndef __LIST_ITERATOR__
#define __LIST_ITERATOR__

class ListIterator {
public:
    typedef int T;
    ListIterator *l, *r;
    T cur;
    bool ok = true;
    virtual void next() = 0;
    virtual ~ListIterator(){};
};
#endif