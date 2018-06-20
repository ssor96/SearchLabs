#include <cstdio>
#include "types.h"

class Writer {
public:
    FILE *f;
    static const int BUF_SIZE = 50000000;
    int p;
    uchar *buf;
    Writer(const char *name);
    void write(int val);
    bool flush();
    void close();
    ~Writer();
};