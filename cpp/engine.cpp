#include <cstdio>
#include "ListIterator.h"
#include "ListIteratorRead.h"
#include "ListIteratorNot.h"
#include "ListIteratorOr.h"
#include "ListIteratorAnd.h"
#include "ListIteratorQuote.h"

int wocabluarySize;
int numberOfArticles = 1454834;
int *sizes, **w;

void buildTree(ListIterator *&cur, char *query, int &pos) {
    if (query[pos] == '&') {
        cur = (ListIterator*)new ListIteratorAnd;
        pos++;
        buildTree(cur->l, query, pos);
        buildTree(cur->r, query, pos);
    }
    else if (query[pos] == '|') {
        cur = (ListIterator*)new ListIteratorOr;
        pos++;
        buildTree(cur->l, query, pos);
        buildTree(cur->r, query, pos);
    }
    else if (query[pos] == '!') {
        cur = (ListIterator*)new ListIteratorNot;
        pos++;
        buildTree(cur->l, query, pos);
    }
    else if (query[pos] == '"') {
        cur = (ListIterator*)new ListIteratorQuote;
        pos++;
        buildTree(cur->l, query, pos);
    }
    else if (query[pos] == '.'){
        pos++;
        int num = 0;
        while (query[pos] >= '0' && query[pos] <= '9') {
            num = num * 10 + query[pos] - '0';
            pos++;
        }
        printf("%d\n", num);
        cur = (ListIterator*)new ListIteratorRead(w[num], sizes[num]);
    }
    else {
        printf("OMG FAILED\n");
    }
    cur->next();
}

int main() {
    FILE *sizesIn = fopen("Index/sizes", "rb");
    if (fread(&wocabluarySize, sizeof(int), 1, sizesIn) != 1) {
        printf("Critical error: error while reading wocabluary size\n");
        return 1;
    }
    printf("%d\n", wocabluarySize);
    sizes = new int[wocabluarySize + 1];
    sizes[0] = 0;
    if (fread(sizes + 1, sizeof(int), wocabluarySize, sizesIn) != wocabluarySize) {
        printf("Critical error: expected %d ints but it's too much\n", wocabluarySize);
        return 1;
    }
    fclose(sizesIn);
    FILE *in = fopen("Index/merged", "rb");
    w = new int*[wocabluarySize + 1];
    for (int i = 1; i <= wocabluarySize; ++i) {
        w[i] = new int[sizes[i]];
        if (fread(w[i], sizeof(int), sizes[i], in) != sizes[i]) {
            printf("Critical error: term #%d expected %d ints but it's too much\n", i, sizes[i]);
            return 1;
        }
    }
    // FILE *stat = fopen("Index/stat");
    // fread(&numberOfArticles, sizeof(int), 1, stat);
    // fclose(stat);
    char buf[65000];
    printf("READY\n");

    while (scanf("%[^\n]s", buf) == 1) {
        getchar();
        ListIterator *queryIterator;
        int pos = 0;
        buildTree(queryIterator, buf, pos);
        while (queryIterator->ok) {
            printf("%d ", queryIterator->cur);
            queryIterator->next();
        }
        printf("\n");
        delete queryIterator;
    }
    delete [] sizes;
    for (int i = 1; i <= wocabluarySize; ++i) {
        delete [] w[i];
    }
    delete [] w;
}