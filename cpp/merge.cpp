#include <cstdio>
#include <cmath>
#include <set>
#include <vector>

struct fileTop{
    int tokId, docId, tf;
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
        if (!fread(&tf, sizeof(int), 1, f)) {
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

double sq(double a) {
    return a * a;
}

int main(int argc, char **argv) {
    std::set<fileTop> st;
    for (int i = 1; i < argc; ++i) {
        st.insert(fileTop(argv[i]));
    }
    int prevVal = 1;
    std::vector<int> sizes;
    int curSz = 0;
    FILE *stat = fopen("Index/stat", "rb");
    int numberOfArticles;
    fread(&numberOfArticles, sizeof(int), 1, stat);
    fclose(stat);
    double *sqLen = new double[numberOfArticles + 1];
    FILE *output = fopen("Index/merged", "wb");
    static int tf[6000000];
    int p = 0;
    while (!st.empty()) {
        fileTop cur = *st.begin();
        st.erase(st.begin());
        if (cur.tokId != prevVal) {
            fwrite(tf, sizeof(int), p, output);
            p = 0;
            sizes.push_back(curSz);
            curSz = 0;
            prevVal = cur.tokId;
        }
        curSz++;
        sqLen[cur.docId] += sq(1 + log(cur.tf));
        fwrite(&cur.docId, sizeof(int), 1, output);
        tf[p++] = cur.tf;
        if (cur.readNext()) {
            st.insert(cur);
        }
        else {
            cur.closeFile();
        }
    }
    fwrite(tf, sizeof(int), p, output);
    fclose(output);
    stat = fopen("Index/stat", "ab");
    for (int i = 1; i <= numberOfArticles; ++i) {
        sqLen[i] = sqrt(sqLen[i]);
    }
    fwrite(sqLen + 1, sizeof(double), numberOfArticles, stat);
    delete [] sqLen;
    sizes.push_back(curSz);
    output = fopen("Index/sizes", "wb");
    curSz = sizes.size();
    fwrite(&curSz, sizeof(int), 1, output);
    fwrite(sizes.data(), sizeof(int), curSz, output);
    fclose(output);
}