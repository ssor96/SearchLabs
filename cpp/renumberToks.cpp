#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <algorithm>

struct info {
    int tokId, docId, tok_pos;
    bool operator <(const info &other) const{
        if (tokId == other.tokId) {
            if (docId == other.docId) {
                return tok_pos < other.tok_pos;
            }
            return docId < other.docId;
        }
        return tokId < other.tokId;
    }
};

int main(int argc, char **argv) {
    struct stat st;
    stat("Index/lemm_table", &st);
    int sz = st.st_size / sizeof(int);
    int *newId = new int[sz + 1];
    newId[0] = 0;
    FILE *f = fopen("Index/lemm_table", "rb");
    fread(newId + 1, sizeof(int), sz, f);
    fclose(f);
    for (int i = 1; i < argc; ++i) {
        stat(argv[i], &st);
        int sz = st.st_size / sizeof(info);
        FILE *f = fopen(argv[i], "rb");
        info *ar = new info[sz];
        fread(ar, sizeof(info), sz, f);
        fclose(f);
        for (int j = 0; j < sz; ++j) {
            ar[j].tokId = newId[ar[j].tokId];
        }
        std::sort(ar, ar + sz);
        argv[i][strlen(argv[i]) - 1 - 6] = 'n';
        f = fopen(argv[i], "wb");
        fwrite(ar, sizeof(info), sz, f);
        fclose(f);
        delete [] ar;
    }
    delete [] newId;
}