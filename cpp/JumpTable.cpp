#include <JumpTable.h>

JumpTable::JumpTable(unsigned char *data) {
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