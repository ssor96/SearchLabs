#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include "ListIterator.h"
#include "ListIteratorRead.h"
#include "ListIteratorNot.h"
#include "ListIteratorOr.h"
#include "ListIteratorAnd.h"
#include "ListIteratorQuote.h"
#include "Reader.h"

using std::map;
using std::vector;
using std::pair;

int numberOfTokens;
int numberOfArticles;
int *df;
unsigned char **docIds, **tf, **coord;
double *len;

void buildTree(ListIterator *&cur, char *query, int &pos, 
               map<int, int> &tfQ, 
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
        cur = (ListIterator*)new ListIteratorRead(docIds[num], df[num]);
    }
    else {
        printf("OMG FAILED\n");
    }
    cur->next();
}

void rankDocs(vector<int> &filtred, map<int, int> &tfQ, 
              vector<pair<pair<double, double>, int>> &res, bool isBoolean) {
    map<int, double> score;
    map<int, double> score2;
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
                double add = (1 + log(it.second)) * (1 + log(tfD)) * idf;
                score[docId] += add;
                score2[docId] += idf;
            }
        }
    }
    res.reserve(score.size());
    for (auto &it: score) {
        res.push_back({{score2[it.first], it.second / len[it.first]}, it.first});
    }
    std::sort(res.begin(), res.end(), std::greater<pair<pair<double, double>, int>>());
}

int main() {
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


    FILE *tfIn = fopen("Index/tf", "rb");
    tf = new unsigned char*[numberOfTokens + 1];
    for (int i = 1; i <= numberOfTokens; ++i) {
        int sz = 0;
        unsigned char tmp;
        do {
            fread(&tmp, sizeof(unsigned char), 1, tfIn);
            sz = (sz << 7) | (tmp & 127);
        } while ((tmp & 128) == 0);
        tf[i] = new unsigned char[sz];
        if (fread(tf[i], sizeof(unsigned char), sz, tfIn) != sz) {
            printf("Critical error: error while reading %d tf list\n", i);
            return 1;
        }
    }
    fclose(tfIn);


    FILE *mainIndexIn = fopen("Index/mainIndex", "rb");
    docIds = new unsigned char*[numberOfTokens + 1];
    for (int i = 1; i <= numberOfTokens; ++i) {
        int sz = 0;
        unsigned char tmp;
        do {
            fread(&tmp, sizeof(unsigned char), 1, mainIndexIn);
            sz = (sz << 7) | (tmp & 127);
        } while ((tmp & 128) == 0);
        docIds[i] = new unsigned char[sz];
        if (fread(docIds[i], sizeof(unsigned char), sz, mainIndexIn) != sz) {
            printf("Critical error: error while reading %d docIds list\n", i);
            return 1;
        }
    }
    fclose(mainIndexIn);


    FILE *coordIn = fopen("Index/coord", "rb");
    coord = new unsigned char*[numberOfTokens + 1];
    for (int i = 1; i <= numberOfTokens; ++i) {
        int sz = 0;
        unsigned char tmp;
        do {
            fread(&tmp, sizeof(unsigned char), 1, coordIn);
            sz = (sz << 7) | (tmp & 127);
        } while ((tmp & 128) == 0);
        printf("%d: expect %d bytes\n", i, sz);
        if (sz == 0) return 0;
        coord[i] = new unsigned char[sz];
        if (fread(coord[i], sizeof(unsigned char), sz, coordIn) != sz) {
            printf("Critical error: error while reading %d coord list\n", i);
            return 1;
        }
    }
    fclose(coordIn);
    

    printf("numOfT = %d numOfA = %d\n", numberOfTokens, numberOfArticles);
    printf("READ\n");
    
    FILE *fdPyCpp, *fdCppPy;
    char *fifoPyCpp = "/tmp/pipePyCpp";
    char *fifoCppPy = "/tmp/pipeCppPy";
    mkfifo(fifoCppPy, 0666);
    const int BUF_SIZE = 500                ;
    char *buf = new char[BUF_SIZE];
    for (;;) {
        fdPyCpp = fopen(fifoPyCpp, "r");
        fscanf(fdPyCpp, "%[^\n]s", buf);
        printf("GET %s\n", buf);
        fgetc(fdPyCpp);
        fclose(fdPyCpp);
        
        ListIterator *queryIterator;
        bool isBoolean = buf[0] - '0';
        int pos = 1;
        map<int, int> tfQ;
        vector<int> dummy;
        buildTree(queryIterator, buf, pos, tfQ, dummy);
        vector<int> filtred;
        if (isBoolean) {
            while (queryIterator->ok) {
                filtred.push_back(queryIterator->cur);
                queryIterator->next();
            }
        }
        vector<pair<pair<double, double>, int>> res;
        rankDocs(filtred, tfQ, res, isBoolean);
        delete queryIterator;
        fdCppPy = fopen(fifoCppPy, "w");
        for (auto &it: res) {
            fprintf(fdCppPy, "%d ", it.second);
        }
        fprintf(fdCppPy, "\n");
        fclose(fdCppPy);
    }
    // unlink(fifoCppPy);
    for (int i = 1; i <= numberOfTokens; ++i) {
        delete [] docIds[i];
        delete [] tf[i];
        delete [] coord[i];
    }
    delete [] docIds;
    delete [] tf;
    delete [] df;
    delete [] len;
    delete [] coord;
    delete [] buf;
}
