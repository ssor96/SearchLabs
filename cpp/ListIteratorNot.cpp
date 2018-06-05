#include "ListIteratorNot.h"

extern const int numberOfArticles;

ListIteratorNot::ListIteratorNot() {
    cur = -1;
}

void ListIteratorNot::next() {
    cur++;
    while (cur < numberOfArticles && l->ok && l->cur == cur) {
        cur++;
        l->next();
    }
    if (cur >= numberOfArticles) ok = false;
}

ListIteratorNot::~ListIteratorNot() {
    delete l;
}
