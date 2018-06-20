#include <cstdio>
#include <vector>
#include "ListIterator.h"
#include "Reader.h"

class ListIteratorQuote : public ListIterator {
private:
    struct Coord;
    Coord *coords;
    int tnum;
    int sz;
    bool check(int);
public:
    void setEnv(std::vector<int>::iterator, std::vector<int>::iterator, int);
    void next();
    ~ListIteratorQuote();
};