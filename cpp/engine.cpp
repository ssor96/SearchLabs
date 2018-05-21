#include <cstdio>

int main() {
    FILE *sizesIn = fopen("Index/sizes", "rb");
    int wocabluarySize;
    if (fread(&wocabluarySize, sizeof(int), 1, sizesIn) != 1) {
        printf("Critical error: error while reading wocabluary size\n");
        return 1;
    }
    int *sizes = new int[wocabluarySize + 1];
    sizes[0] = 0;
    if (fread(sizes + 1, sizeof(int), wocabluarySize, sizesIn) != wocabluarySize) {
        printf("Critical error: expected %d ints but it's too much\n", wocabluarySize);
        return 1;
    }
    fclose(sizesIn);
    FILE *in = fopen("Index/merged", "rb");
    int **w = new int*[wocabluarySize + 1];
    for (int i = 1; i <= wocabluarySize; ++i) {
        w[i] = new int[sizes[i]];
        if (fread(w[i], sizeof(int), sizes[i], in) != sizes[i]) {
            printf("Critical error: term #%d expected %d ints but it's too much\n", i, sizes[i]);
            return 1;
        }
    }
    printf("READY\n");
    int idx;
    while (scanf("%d", &idx) == 1) {
        printf("%d: ", sizes[idx]);
        for (int i = 0; i < sizes[idx]; ++i) {
            printf("%d ", w[idx][i]);
        }
        printf("\n");
    }
}