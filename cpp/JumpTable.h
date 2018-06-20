#include "Reader.h"
#include "types.h"

class JumpTable {
private:
    Reader reader;
    int val, p;
public:
    void init(uchar *data);
    void next();
    int getVal() {
        return val;
    }
    int getPos() {
        return p;
    }
};