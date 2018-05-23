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

using std::map;
using std::vector;
using std::pair;

int wocabluarySize;
int numberOfArticles;
int *df, *sizes, **w, **tf;
double *len;

void buildTree(ListIterator *&cur, char *query, int &pos, map<int, int> &tfQ) {
    if (query[pos] == '&') {
        cur = (ListIterator*)new ListIteratorAnd;
        pos++;
        buildTree(cur->l, query, pos, tfQ);
        buildTree(cur->r, query, pos, tfQ);
    }
    else if (query[pos] == '|') {
        cur = (ListIterator*)new ListIteratorOr;
        pos++;
        buildTree(cur->l, query, pos, tfQ);
        buildTree(cur->r, query, pos, tfQ);
    }
    else if (query[pos] == '!') {
        cur = (ListIterator*)new ListIteratorNot;
        pos++;
        buildTree(cur->l, query, pos, tfQ);
    }
    else if (query[pos] == '"') {
        cur = (ListIterator*)new ListIteratorQuote;
        pos++;
        buildTree(cur->l, query, pos, tfQ);
    }
    else if (query[pos] == '.'){
        pos++;
        int num = 0;
        while (query[pos] >= '0' && query[pos] <= '9') {
            num = num * 10 + query[pos] - '0';
            pos++;
        }
        tfQ[num]++;
        cur = (ListIterator*)new ListIteratorRead(w[num], sizes[num]);
    }
    else {
        printf("OMG FAILED\n");
    }
    cur->next();
}

void rankDocs(vector<int> &filtred, map<int, int> &tfQ, vector<pair<double, int>> &res, bool isBoolean) {
    map<int, double> score;
    for (auto &it: tfQ) {
        for (int i = 0; i < sizes[it.first]; ++i) {
            int docId = w[it.first][i];
            int tfD = tf[it.first][i];
            auto it2 = lower_bound(filtred.begin(), filtred.end(), docId);
            if ((!isBoolean && tfD) || (it2 != filtred.end() && *it2 == docId)) {
                score[docId] += (1 + log(it.second)) * (1 + log(tfD)) * numberOfArticles / df[it.first];
            }
        }
    }
    res.reserve(score.size());
    for (auto &it: score) {
        res.push_back({it.second / len[it.first], it.first});
    }
    std::sort(res.begin(), res.end(), std::greater<pair<double, int>>());
}

int main() {
    FILE *sizesIn = fopen("Index/sizes", "rb");
    if (fread(&wocabluarySize, sizeof(int), 1, sizesIn) != 1) {
        printf("Critical error: error while reading wocabluary size\n");
        return 1;
    }
    sizes = new int[wocabluarySize + 1];
    sizes[0] = 0;
    if (fread(sizes + 1, sizeof(int), wocabluarySize, sizesIn) != wocabluarySize) {
        printf("Critical error: expected %d ints but it's too much\n", wocabluarySize);
        return 1;
    }
    fclose(sizesIn);
    

    FILE *in = fopen("Index/merged", "rb");
    w = new int*[wocabluarySize + 1];
    tf = new int*[wocabluarySize + 1];
    for (int i = 1; i <= wocabluarySize; ++i) {
        w[i] = new int[sizes[i]];
        if (fread(w[i], sizeof(int), sizes[i], in) != sizes[i]) {
            printf("Critical error: term #%d expected %d ints but it's too much\n", i, sizes[i]);
            return 1;
        }
        tf[i] = new int[sizes[i]];
        if (fread(tf[i], sizeof(int), sizes[i], in) != sizes[i]) {
            printf("Critical error: tf term #%d expected %d ints but it's too much\n", i, sizes[i]);
            return 1;
        }
    }
    

    FILE *stat = fopen("Index/stat", "rb");
    df = new int[wocabluarySize + 1];
    len = new double[numberOfArticles + 1];
    fread(&numberOfArticles, sizeof(int), 1, stat);
    fread(df + 1, sizeof(int), wocabluarySize, stat);
    fread(len + 1, sizeof(double), numberOfArticles, stat);
    fclose(stat);
    
    printf("READ\n");
    
    FILE *fdPyCpp, *fdCppPy;
    char *fifoPyCpp = "/tmp/pipePyCpp";
    char *fifoCppPy = "/tmp/pipeCppPy";
    mkfifo(fifoCppPy, 0666);
    const int BUF_SIZE = 500000;
    char buf[BUF_SIZE];
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
        buildTree(queryIterator, buf, pos, tfQ);
        vector<int> filtred;
        if (isBoolean) {
            while (queryIterator->ok) {
                filtred.push_back(queryIterator->cur);
                queryIterator->next();
            }
        }
        vector<pair<double, int>> res;
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
    delete [] sizes;
    for (int i = 1; i <= wocabluarySize; ++i) {
        delete [] w[i];
    }
    delete [] w;
}