#include "ListIteratorRead.h"

void ListIteratorRead::next() {
    if (pos == sz) {
        ok = false;
    }
    else {
        cur = reader.getNext();
        pos++;
    }
}