#include <cstdio>
#include <set>
#include <vector>

struct fileTop{
    int tokId, docId;
    FILE* f;
    fileTop(){}
    fileTop(char *name) {
        f = fopen(name, "rb");
        readNext();
    }
    bool readNext() {
        if (!fread(&tokId, sizeof(int), 1, f)) {
            return false;
        }
        if (!fread(&docId, sizeof(int), 1, f)) {
            return false;
        }
        return true;
    }
    bool operator <(const fileTop &other) const{
        if (tokId == other.tokId) {
            return docId < other.docId;
        }
        return tokId < other.tokId;
    }
    void closeFile() {
        fclose(f);
    }
};

int main(int argc, char **argv) {
    std::set<fileTop> st;
    for (int i = 1; i < argc; ++i) {
        st.insert(fileTop(argv[i]));
    }
    int prevVal = 1;
    std::vector<int> sizes;
    int curSz = 0;
    FILE *output = fopen("Index/merged", "wb");
    while (!st.empty()) {
        fileTop cur = *st.begin();
        st.erase(st.begin());
        if (cur.tokId != prevVal) {
            sizes.push_back(curSz);
            curSz = 0;
            prevVal = cur.tokId;
        }
        curSz++;
        fwrite(&cur.docId, sizeof(int), 1, output);
        if (cur.readNext()) {
            st.insert(cur);
        }
        else {
            cur.closeFile();
        }
    }
    fclose(output);
    sizes.push_back(curSz);
    output = fopen("Index/sizes", "wb");
    curSz = sizes.size();
    fwrite(&curSz, sizeof(int), 1, output);
    fwrite(sizes.data(), sizeof(int), curSz, output);
    fclose(output);
}