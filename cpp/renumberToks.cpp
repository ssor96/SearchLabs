#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>

struct info {
    int tokId, docId, tf;
    bool operator <(const info &other) const{
        if (tokId == other.tokId) {
            return docId < other.docId;
        }
        return tokId < other.tokId;
    }
};

int main(int argc, char **argv) {
    std::vector<int> newId;
    newId.push_back(0);
    FILE *f = fopen("Index/lemm_table", "rb");
    int id;
    while (fread(&id, sizeof(int), 1, f)) {
        newId.push_back(id);
    }
    fclose(f);
    for (int i = 1; i < argc; ++i) {
        FILE *f = fopen(argv[i], "rb");
        std::vector<info> ar;
        for (;;) {
            int tokId, docId, tf;
            if (!fread(&tokId, sizeof(int), 1, f)) {
                break;
            }
            if (!fread(&docId, sizeof(int), 1, f)) {
                break;
            }
            if (!fread(&tf, sizeof(int), 1, f)) {
                break;
            }
            ar.push_back({newId[tokId], docId, tf});
        }
        fclose(f);
        std::sort(ar.begin(), ar.end());
        argv[i][strlen(argv[i]) - 1 - 6] = 'n';
        f = fopen(argv[i], "wb");
        info p = ar[0];
        p.tf = 0;
        for (info &inf: ar) {
            if (inf.tokId == p.tokId && inf.docId == p.docId) {
                p.tf += inf.tf;
            }
            else {
                fwrite(&p.tokId, sizeof(int), 1, f);
                fwrite(&p.docId, sizeof(int), 1, f);
                fwrite(&p.tf, sizeof(int), 1, f);
                p = inf;
            }
        }
        fwrite(&p.tokId, sizeof(int), 1, f);
        fwrite(&p.docId, sizeof(int), 1, f);
        fwrite(&p.tf, sizeof(int), 1, f);
        fclose(f);
    }
}