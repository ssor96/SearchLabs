#include "ListIteratorQuote.h"

extern const int *df, **docIds, **tf;

struct ListIteratorQuote::Coord {
    int *data;
    FILE *source;
    int id, curDoc, curPos;
    Coord() {
        const int BUF_SIZE = 1000000;
        data = new int[BUF_SIZE];
        source = fopen("Index/coord", "rb");
        if (source == NULL) {
            printf("Cannot open file!\n");
        }
    }
    void set(int tokId) {
        id = tokId;
        for (int j = 1; j < id; ++j) {
            for (int k = 0; k < df[j]; ++k) {
                if (fread(data, sizeof(int), tf[j][k], source) != tf[j][k]) {
                    // something bad happens
                }
            }
        }
        if (fread(data, sizeof(int), tf[id][0], source) != tf[id][0]) {
            // something bad happens
        }
        curDoc = 0;
        curPos = 0;
    }
    bool next() {
        if (curPos + 1 == tf[id][curDoc]) {
            if (!nextDoc()) {
                return false;
            }
        }
        else {
            curPos++;
        }
        return true;
    }
    bool nextDoc() {
        if (curDoc + 1 == df[id]) {
            return false;
        }
        else {
            curDoc++;
            fread(data, sizeof(int), tf[id][curDoc], source);
        }
        curPos = 0;
        return true;
    }
    int getPos() {
        return data[curPos];
    }
    int getDocId() {
        return docIds[id][curDoc];
    }
    ~Coord() {
        fclose(source);
        delete [] data;
    }
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