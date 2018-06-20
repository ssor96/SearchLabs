#ifndef __READER_H__
#define __READER_H__

#include "types.h"

class Reader {
protected:
    int readInt();
public:
    const uchar *data;
    int p;
    Reader() {
        p = 0;
    }
    int getNext();
};

class DiffReader: public Reader {
public:
    int prev;
    DiffReader() {
        prev = p = 0;
    }
    int getNext();
};

#endif