#include "JumpTable.h"

void JumpTable::init(uchar *data) {
    val = 1;
    reader.data = data;
    next();
}

void JumpTable::next() {
    if (!val) return;
    val = reader.getNext();
    if (val) {
        p = reader.getNext();
    }
}