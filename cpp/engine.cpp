#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cmath>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>
#include <chrono>
#include "ListIterator.h"
#include "ListIteratorRead.h"
#include "ListIteratorNot.h"
#include "ListIteratorOr.h"
#include "ListIteratorAnd.h"
#include "ListIteratorQuote.h"
#include "Reader.h"
#include "types.h"

using std::unordered_map;
using std::map;
using std::vector;
using std::pair;

int numberOfTokens;
int numberOfArticles;
int *df, *byteLen, *titleLen;
uchar **docIds, **tf, **coord, **jumps;
double *len;
map<int, int> *titleToks;

void buildTree(ListIterator *&cur, char *query, int &pos, 
               unordered_map<int, int> &tfQ, 
               vector<int> &orderedToks) {
    if (query[pos] == '&') {
        cur = (ListIterator*)new ListIteratorAnd;
        pos++;
        buildTree(cur->l, query, pos, tfQ, orderedToks);
        buildTree(cur->r, query, pos, tfQ, orderedToks);
    }
    else if (query[pos] == '|') {
        cur = (ListIterator*)new ListIteratorOr;
        pos++;
        buildTree(cur->l, query, pos, tfQ, orderedToks);
        buildTree(cur->r, query, pos, tfQ, orderedToks);
    }
    else if (query[pos] == '!') {
        cur = (ListIterator*)new ListIteratorNot;
        pos++;
        buildTree(cur->l, query, pos, tfQ, orderedToks);
    }
    else if (query[pos] == '"') {
        ListIteratorQuote *tmp = new ListIteratorQuote;
        cur = (ListIterator*)tmp;
        pos++;
        int num = 0;
        while (query[pos] >= '0' && query[pos] <= '9') {
            num = num * 10 + query[pos] - '0';
            pos++;
        }
        int ppos = orderedToks.size();
        buildTree(cur->l, query, pos, tfQ, orderedToks);
        tmp->setEnv(orderedToks.begin() + ppos, orderedToks.end(), num);
    }
    else if (query[pos] == '.') {
        pos++;
        int num = 0;
        while (query[pos] >= '0' && query[pos] <= '9') {
            num = num * 10 + query[pos] - '0';
            pos++;
        }
        orderedToks.push_back(num);
        tfQ[num]++;
        cur = (ListIterator*)new ListIteratorRead(docIds[num], jumps[num], byteLen[num]);
    }
    else {
        printf("OMG FAILED\n");
    }
    cur->next();
}

struct rankKey {
    double idfSum, tfIdf;
    bool operator < (const rankKey &other) const {
        if (fabs(idfSum - other.idfSum) < 1e-8) {
            return tfIdf < other.tfIdf;
        }
        return idfSum < other.idfSum;
    }
};

inline double tfIdfFormula(int tf1, int tf2, double idf) {
    return (1 + log(tf1)) * (1 + log(tf2)) * idf;
}

void rankDocs(vector<int> &filtred, unordered_map<int, int> &tfQ, 
              vector<pair<rankKey, int>> &res, bool isBoolean) {
    unordered_map<int, double> score;
    unordered_map<int, double> score2;
    if (isBoolean) {
        for (int docId: filtred) {
            score[docId] = 0;
        }
    }
    for (auto &it: tfQ) {
        if (it.first == 0) continue;
        double idf = 1.0 * numberOfArticles / df[it.first];
        DiffReader readerDocId;
        readerDocId.data = docIds[it.first];
        Reader readerTf;
        readerTf.data = tf[it.first];
        for (int i = 0; i < df[it.first]; ++i) {
            int docId = readerDocId.getNext();
            int tfD = readerTf.getNext();
            auto it2 = lower_bound(filtred.begin(), filtred.end(), docId);
            if ((!isBoolean && tfD) || (it2 != filtred.end() && *it2 == docId)) {
                double add = tfIdfFormula(it.second, tfD, idf);
                score[docId] += add;
                score2[docId] += idf;
                if (titleToks[docId].find(it.first) != titleToks[docId].end()) {
                    score2[docId] += it.second * titleToks[docId][it.first] * idf / titleLen[docId];
                }
            }
        }
    }
    res.reserve(score.size());
    for (auto &it: score) {
        res.push_back({{score2[it.first], it.second / len[it.first]}, it.first});
    }
    std::sort(res.begin(), res.end(), std::greater<pair<rankKey, int>>());
}

uchar** readData(const char *fileName, int *bLen, int dataSz) {
    FILE *f = fopen(fileName, "rb");
    uchar **data = new uchar*[dataSz + 1];
    uchar tmp;
    for (int i = 1; i <= dataSz; ++i) {
        int sz = 0;
        do {
            fread(&tmp, sizeof(uchar), 1, f);
            sz = (sz << 7) | (tmp & 127);
        } while ((tmp & 128) == 0);
        bLen[i] = sz;
        data[i] = new uchar[sz];
        if (fread(data[i], sizeof(uchar), sz, f) != sz) {
            printf("Critical error: error while reading %d data list\n", i);
            exit(1);
        }
    }
    fclose(f);
    return data;
}

int main() {
    {
        FILE *stat = fopen("Index/stat", "rb");
        if (fread(&numberOfArticles, sizeof(int), 1, stat) != 1) {
            printf("Critical error: error while reading numberOfArticles\n");
            return 1;
        }
        if (fread(&numberOfTokens, sizeof(int), 1, stat) != 1) {
            printf("Critical error: error while reading numberOfTokens\n");
            return 1;
        }
        len = new double[numberOfArticles + 1];
        if (fread(len + 1, sizeof(double), numberOfArticles, stat) != numberOfArticles) {
            printf("Critical error: error while reading Articles len\n");
            return 1;
        }
        fclose(stat);


        FILE *dfIn = fopen("Index/df", "rb");
        df = new int[numberOfTokens + 1];
        if (fread(df + 1, sizeof(int), numberOfTokens, dfIn) != numberOfTokens) {
            printf("Critical error: error while reading df\n");
            return 1;
        }
        fclose(dfIn);
    }

    byteLen = new int[numberOfTokens + 1];

    tf = readData("Index/tf", byteLen, numberOfTokens);

    coord = readData("Index/coord", byteLen, numberOfTokens);

    jumps = readData("Index/jumpTables", byteLen, numberOfTokens);

    docIds = readData("Index/mainIndex", byteLen, numberOfTokens);

    byteLen[0] = 0;
    jumps[0] = new uchar[1];
    jumps[0][0] = 128;

    {
        struct stat st;
        stat("Index/titles_toks", &st);
        int sz = st.st_size / sizeof(int);
        int *titleTokData = new int[sz];
        
        FILE *f = fopen("Index/titles_toks", "rb");
        fread(titleTokData, sizeof(int), sz, f);
        fclose(f);

        titleLen = new int[numberOfArticles + 1];
        titleToks = new map<int, int>[numberOfArticles + 1];

        int p = 0;
        for (int i = 1; i <= numberOfArticles; ++i) {
            titleLen[i] = p;
            for (; titleTokData[p]; ++p) {
                titleToks[i][titleTokData[p]]++;
            }
            titleLen[i] = p - titleLen[i];
            ++p;
        }
        delete [] titleTokData;
    }

    printf("numOfT = %d numOfA = %d\n", numberOfTokens, numberOfArticles);
    printf("READ\n");
    
    FILE *fdPyCpp, *fdCppPy;
    const char *fifoPyCpp = "/tmp/pipePyCpp";
    const char *fifoCppPy = "/tmp/pipeCppPy";
    mkfifo(fifoCppPy, 0666);
    const int BUF_SIZE = 500                ;
    char *buf = new char[BUF_SIZE];
    for (;;) {
        fdPyCpp = fopen(fifoPyCpp, "r");
        if (fdPyCpp == NULL) {
            printf("Cannot open pyCpp pipe. Try to start web before engine\n");
            break;
        }
        fscanf(fdPyCpp, "%[^\n]s", buf);
        printf("GET %s\n", buf);
        fgetc(fdPyCpp);
        fclose(fdPyCpp);
        
        auto start = std::chrono::steady_clock::now();

        ListIterator *queryIterator;
        bool isBoolean = buf[0] - '0';
        int pos = 1;
        unordered_map<int, int> tfQ;
        vector<int> dummy;
        buildTree(queryIterator, buf, pos, tfQ, dummy);
        vector<int> filtred;
        if (isBoolean) {
            while (queryIterator->ok) {
                filtred.push_back(queryIterator->cur);
                queryIterator->next();
            }
        }
        vector<pair<rankKey, int>> res;
        rankDocs(filtred, tfQ, res, isBoolean);
        delete queryIterator;

        auto end = std::chrono::steady_clock::now();

        printf("%lf ms\n", std::chrono::duration<double, std::milli>(end - start).count());

        fdCppPy = fopen(fifoCppPy, "w");
        for (auto &it: res) {
            fprintf(fdCppPy, "%d ", it.second);
        }
        fprintf(fdCppPy, "\n");
        fclose(fdCppPy);
    }
    // unlink(fifoCppPy);
    delete [] jumps[0];
    for (int i = 1; i <= numberOfTokens; ++i) {
        delete [] tf[i];
        delete [] docIds[i];
        delete [] coord[i];
        delete [] jumps[i];
    }
    delete [] len;
    delete [] df;
    delete [] byteLen;
    delete [] tf;
    delete [] docIds;
    delete [] coord;
    delete [] jumps;
    delete [] titleToks;
    delete [] buf;
}
