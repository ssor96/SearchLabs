#include <cstdio>

class Writer {
public:
    FILE *f;
    static const int BUF_SIZE = 50000000;
    int p;
    unsigned char *buf;
    Writer(char *name);
    void write(int val);
    bool flush();
    void close();
    ~Writer();
};