#include "ListIteratorRead.h"

ListIteratorRead::ListIteratorRead(uchar *readerData, uchar *jumpTableData, int sz):
                                                sz(sz) {
    pos = 0;
    reader.data = readerData;
    jumpTable.init(jumpTableData);
}

void ListIteratorRead::next() {
    if (reader.p == sz) {
        ok = false;
    }
    else {
        cur = reader.getNext();
        if (reader.p == jumpTable.getPos()) {
            jumpTable.next();
        }
    }
}

bool ListIteratorRead::jump(int mx) {
    while (jumpTable.getVal() && jumpTable.getVal() <= mx) {
        reader.setPos(jumpTable.getPos());
        cur = jumpTable.getVal();
        jumpTable.next();
    }
    return cur == mx;
}