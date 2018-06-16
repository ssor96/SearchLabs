#include "ListIteratorQuote.h"

extern const int *df;
extern const unsigned char **docIds, **tf, **coord;

struct ListIteratorQuote::Coord {
    DiffReader readerC;
    DiffReader readerDocId;
    Reader readerTf;
    int readedTf, readedDocId, readedC;
    int id, curDoc, curPos;
    Coord() {}
    void set(int tokId) {
        id = tokId;
        readerC.data = coord[id];
        readedC = readerC.getNext();
        readerDocId.data = docIds[id];
        readedDocId = readerDocId.getNext();
        readerTf.data = tf[id];
        readedTf = readerTf.getNext();
        curDoc = 0;
        curPos = 0;
    }
    bool next() {
        if (curPos + 1 == readedTf) {
            if (!nextDoc()) {
                return false;
            }
        }
        else {
            readedC = readerC.getNext();
            curPos++;
        }
        return true;
    }
    bool nextDoc() {
        if (curDoc + 1 == df[id]) {
            return false;
        }
        else {
            while (curPos + 1 != readedTf) {
                readerC.getNext();
                curPos++;
            }        
            curDoc++;
            readedTf = readerTf.getNext();
            readedDocId = readerDocId.getNext();
            readerC.prev = 0;
        }
        curPos = 0;
        return true;
    }
    int getPos() {
        return readedC;
    }
    int getDocId() {
        return readedDocId;
    }
    ~Coord() {}
};

void ListIteratorQuote::setEnv(std::vector<int>::iterator beg, 
                               std::vector<int>::iterator end, 
                               int _sz) {
    sz = _sz;
    tnum = end - beg;
    coords = new Coord[tnum];
    for (int i = 0; i < tnum; ++i, ++beg) {
        coords[i].set(*beg);
    }
}

bool ListIteratorQuote::check(int needDocId) {
    for (int i = 0; i < tnum; ++i) {
        while (coords[i].getDocId() != needDocId) {
            coords[i].nextDoc();
        }
    }
    bool found = false;
    while (!found) {
        int prev = coords[0].getPos();
        for (int i = 1; i < tnum; ++i) {
            while (coords[i].getPos() < prev) {
                if (!coords[i].next()) {
                    return false;
                }
                if (coords[i].getDocId() > needDocId) {
                    return false;
                }
            }
            prev = coords[i].getPos();
        }
        if (coords[tnum - 1].getPos() - coords[0].getPos() <= sz) {
            found = true;
            break;
        }
        else {
            if (!coords[0].next()) {
                break;
            }
            if (coords[0].getDocId() > needDocId) {
                break;
            }
        }
    }
    return found;
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
    l->next();
}

ListIteratorQuote::~ListIteratorQuote() {
    delete l;
    delete [] coords;
}