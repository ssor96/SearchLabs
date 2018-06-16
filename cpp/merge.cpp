#include <cstdio>
#include <cmath>
#include <set>
#include "Writer.h"

struct fileTop {
    int tokId, docId, tok_pos;
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
        if (!fread(&tok_pos, sizeof(int), 1, f)) {
            return false;
        }
        return true;
    }
    bool operator <(const fileTop &other) const{
        if (tokId == other.tokId) {
            if (docId == other.docId) {
                return tok_pos < other.tok_pos;
            }
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
    int prevTokId = 1;
    int prevDocId = 1;
    FILE *stat = fopen("Index/stat", "rb");
    int numberOfArticles;
    fread(&numberOfArticles, sizeof(int), 1, stat);
    fclose(stat);
    double *sqLen = new double[numberOfArticles];
    Writer mainIndex("Index/mainIndex");
    int prevMI = 0;
    Writer tfOut("Index/tf");
    Writer coord("Index/coord");
    int prevC = 0;
    FILE *dfOut = fopen("Index/df", "wb");
    static int bufDf[6000000];
    int tf = 0;
    int df = 0;
    int pDf = 0;
    while (!st.empty()) {
        fileTop cur = *st.begin();
        st.erase(st.begin());
        if (cur.tokId != prevTokId || cur.docId != prevDocId) {
            tfOut.write(tf);
            mainIndex.write(prevDocId - prevMI - 1);
            sqLen[prevDocId] += sq(tf);
            prevMI = prevDocId;
            prevDocId = cur.docId;
            df++;
            tf = 0;
            prevC = 0;
        }
        if (cur.tokId != prevTokId) {
            coord.flush();
            tfOut.flush();
            mainIndex.flush();
            bufDf[pDf++] = df;
            prevTokId = cur.tokId;
            prevMI = 0;
            df = 0;
        }
        coord.write(cur.tok_pos - prevC - 1);
        prevC = cur.tok_pos;
        tf++;
        if (cur.readNext()) {
            st.insert(cur);
        }
        else {
            cur.closeFile();
        }
    }

    tfOut.write(tf);
    mainIndex.write(prevDocId - prevMI - 1);
    coord.flush();
    tfOut.flush();
    mainIndex.flush();

    sqLen[prevDocId] += sq(tf);
    bufDf[pDf++] = df;

    fwrite(bufDf, sizeof(int), pDf, dfOut);


    mainIndex.close();
    tfOut.close();
    coord.close();
    fclose(dfOut);
    
    stat = fopen("Index/stat", "ab");
    for (int i = 0; i < numberOfArticles; ++i) {
        sqLen[i] = sqrt(sqLen[i]);
    }
    fwrite(&prevTokId, sizeof(int), 1, stat);
    fwrite(sqLen, sizeof(double), numberOfArticles, stat);
    delete [] sqLen;
}