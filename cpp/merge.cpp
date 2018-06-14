#include <cstdio>
#include <cmath>
#include <set>
#include <vector>

struct fileTop{
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
    int prevDocId = 0;
    FILE *stat = fopen("Index/stat", "rb");
    int numberOfArticles;
    fread(&numberOfArticles, sizeof(int), 1, stat);
    fclose(stat);
    double *sqLen = new double[numberOfArticles];
    FILE *mainIndex = fopen("Index/mainIndex", "wb");
    FILE *tfOut = fopen("Index/tf", "wb");
    FILE *coord = fopen("Index/coord", "wb");
    FILE *dfOut = fopen("Index/df", "wb");
    const int BUF_SIZE = 1000000;
    static int buf[BUF_SIZE];
    static int bufDf[6000000];
    static int bufTf[2000000];
    static int bufDocIds[2000000];
    int bufp = 0;
    int df = 0;
    int pDf = 0;
    int pTf = 0;
    int pDocIds = 0;
    while (!st.empty()) {
        fileTop cur = *st.begin();
        st.erase(st.begin());
        if (cur.tokId != prevTokId || cur.docId != prevDocId) {
            bufTf[pTf++] = bufp;
            bufDocIds[pDocIds++] = prevDocId;
            fwrite(buf, sizeof(int), bufp, coord);
            df++;
            sqLen[prevDocId] += sq(bufp);
            prevDocId = cur.docId;
            bufp = 0;
        }
        if (cur.tokId != prevTokId) {
            fwrite(bufTf, sizeof(int), pTf, tfOut);
            fwrite(bufDocIds, sizeof(int), pDocIds, mainIndex);
            pTf = pDocIds = 0;
            bufDf[pDf++] = df;
            prevTokId = cur.tokId;
            df = 0;
        }
        buf[bufp++] = cur.tok_pos;
        if (cur.readNext()) {
            st.insert(cur);
        }
        else {
            cur.closeFile();
        }
    }

    bufTf[pTf++] = bufp;
    bufDocIds[pDocIds++] = prevDocId;
    fwrite(buf, sizeof(int), bufp, coord);
    sqLen[prevDocId] += sq(bufp);

    fwrite(bufTf, sizeof(int), pTf, tfOut);
    fwrite(bufDocIds, sizeof(int), pDocIds, mainIndex);
    bufDf[pDf++] = df;

    fwrite(bufDf, sizeof(int), pDf, dfOut);


    fclose(mainIndex);
    fclose(tfOut);
    fclose(coord);
    fclose(dfOut);
    
    stat = fopen("Index/stat", "ab");
    for (int i = 0; i < numberOfArticles; ++i) {
        sqLen[i] = sqrt(sqLen[i]);
    }
    fwrite(&prevTokId, sizeof(int), 1, stat);
    fwrite(sqLen, sizeof(double), numberOfArticles, stat);
    delete [] sqLen;
}