#include "ListIteratorQuote.h"

bool check(int a) {
    return true;
}

void ListIteratorQuote::next() {
    while (l->ok && !check(l->cur)) {
        l->next();
    }
    if (!l->ok) {
        ok = false;
        return;
    }
    cur = l->cur;
}

ListIteratorQuote::~ListIteratorQuote() {
    delete l;
}