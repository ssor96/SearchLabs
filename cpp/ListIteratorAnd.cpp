#include "ListIteratorAnd.h"

void ListIteratorAnd::next() {
    while (l->ok && r->ok) {
        if (l->cur < r->cur) {
            l->next();
        }
        else if (r->cur < l->cur) {
            r->next();
        }
        else break;
    }
    if (!l->ok || !r->ok) {
        ok = false;
        return;
    }
    cur = l->cur;
    l->next();
    r->next();
}

ListIteratorAnd::~ListIteratorAnd() {
    delete l;
    delete r;
}