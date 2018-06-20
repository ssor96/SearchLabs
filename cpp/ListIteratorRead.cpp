#include "ListIteratorRead.h"

ListIteratorRead::ListIteratorRead(uchar *readerData, uchar *jumpTableData, int sz):
                                                sz(sz) {
    pos = 0;
    reader.data = readerData;
    jumpTable.init(jumpTableData);
}

void ListIteratorRead::next() {
    if (pos == sz) {
        ok = false;
    }
    else {
        cur = reader.getNext();
        pos++;
    }
}