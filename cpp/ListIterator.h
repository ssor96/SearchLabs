#ifndef __LIST_ITERATOR__
#define __LIST_ITERATOR__

#include "types.h"

class ListIterator {
public:
    typedef int T;
    ListIterator *l, *r;
    T cur;
    bool ok = true;
    virtual void next() = 0;
    bool jump(int mx) {
    	return false;
    }
    virtual ~ListIterator(){};
};
#endif