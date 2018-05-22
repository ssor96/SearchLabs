#include "ListIteratorOr.h"

void ListIteratorOr::next() {
    if (!l->ok && !r->ok) {
        ok = false;
    }
    else if (!l->ok) {
        cur = r->cur;
        r->next();
    }
    else if (!r->ok) {
        cur = l->cur;
        l->next();
    }
    else { // l and r is valid now
        if (l->cur < r->cur) {
            cur = l->cur;
            l->next();
        }
        else if (r->cur < l->cur) {
            cur = r->cur;
            r->next();
        }
        else {
            cur = l->cur;
            l->next();
            r->next();
        }
    }
}

ListIteratorOr::~ListIteratorOr() {
    delete l;
}