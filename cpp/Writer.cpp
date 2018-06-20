#include "Writer.h"

Writer::Writer(const char *name) {
    buf = new uchar[BUF_SIZE];
    f = fopen(name, "wb");
    p = 4;
}

void Writer::write(int val) {
    bool was = false;
    for (int j = 4; j >= 1; --j) {
        uchar tmp = (val >> (7 * j)) & 127;
        if (tmp || was) {
            was = true;
            buf[p++] = tmp;
        }
    }
    buf[p++] = (val & 127) | 128;
}

bool Writer::flush() {
    int en = p;
    int beg;
    for (beg = 0; en - 4 < (1 << 7 * (3 - beg)); ++beg);
    p = beg;
    write(en - 4);
    if (fwrite(buf + beg, sizeof(unsigned char), en - beg, f) != en - beg) {
        return false;
    }
    p = 4;
    return true;
}

void Writer::close() {
    fclose(f);
    f = NULL;
}

Writer::~Writer() {
    delete [] buf;
    if (f) {
        close();
    }
}