#include <Reader.h>

class JumpTable {
private:
    Reader reader;
    int val, p;
public:
    JumpTable(unsigned char *data);
    void next();
    int getVal() {
        return val;
    }
    int getPos() {
        return p;
    }
};