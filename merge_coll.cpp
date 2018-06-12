#include <cstdio>
#include <cmath>
#include <set>
#include <vector>

struct fileTop{
    int l, r, c;
    FILE* f;
    fileTop(){}
    fileTop(char *name) {
        f = fopen(name, "rb");
        readNext();
    }
    bool readNext() {
        if (!fread(&l, sizeof(int), 1, f)) {
            return false;
        }
        if (!fread(&r, sizeof(int), 1, f)) {
            return false;
        }
        if (!fread(&c, sizeof(int), 1, f)) {
            return false;
        }
        return true;
    }
    bool operator <(const fileTop &other) const{
        if (l == other.l) {
            if (r == other.r) return f < other.f;
            return r < other.r;
        }
        return l < other.l;
    }
    void closeFile() {
        fclose(f);
    }
};

int main(int argc, char **argv) {
    std::set<fileTop> st;
    for (int i = 1; i < argc; ++i) {
        st.insert(fileTop(argv[i]));
    }
    int pl = st.begin()->l;
    int pr = st.begin()->r;
    long long curCnt = 0;
    // FILE *stat = fopen("Index/stat", "rb");
    // int numberOfArticles;
    // fread(&numberOfArticles, sizeof(int), 1, stat);
    // fclose(stat);
    long long all = 0;
    int mx = 0;
    while (!st.empty()) {
        fileTop cur = *st.begin();
        st.erase(st.begin());
        if (cur.l == pl && cur.r == pr) {
            curCnt += cur.c;
        }
        else {
            if (cur.l < pl) {
                printf("OBOSRALSYA %d %d %d\n", cur.l, cur.r, cur.c);
                return 0;
            }
            all++;
            printf("%d %d %lld\n", pl, pr, curCnt);
            pl = cur.l;
            pr = cur.r;
            curCnt = cur.c;
        }
        if (cur.readNext()) {
            st.insert(cur);
        }
    }
    all++;
    printf("%d %d %lld\n", pl, pr, curCnt);
    printf("All = %lld\n", all);
}