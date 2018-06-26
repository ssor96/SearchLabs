#include <cstdio>
#include <cmath>
#include <cstring>
#include <set>
#include <vector>
#include <algorithm>

struct fileTop{
    int l, r;
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
    	FILE *f = fopen(argv[i], "rb");
    	std::vector<std::pair<int, int>> ar;
    	int l, r;
    	for (;;) {
    		if (!fread(&l, sizeof(int), 1, f)) {
    			break;
    		}
    		fread(&r, sizeof(int), 1, f);
    		ar.push_back({l, r});
    	}
    	fclose(f);
    	argv[i][strlen(argv[i]) - 1 - 6] = 'n';
    	f = fopen(argv[i], "wb");
    	std::sort(ar.begin(), ar.end());
    	for (auto &it: ar) {
    		fwrite(&it.first, sizeof(int), 1, f);
    		fwrite(&it.second, sizeof(int), 1, f);
    	}
    	fclose(f);
        st.insert(fileTop(argv[i]));
    }
    int pl = st.begin()->l;
    int pr = st.begin()->r;
    long long curCnt = 0;
    FILE *f = fopen("stat/summed_col", "wb");
    long long all = 0;
    while (!st.empty()) {
        fileTop cur = *st.begin();
        st.erase(st.begin());
        if (cur.l == pl && cur.r == pr) {
            curCnt++;
        }
        else {
            fwrite(&pl, sizeof(int), 1, f);
            fwrite(&pr, sizeof(int), 1, f);
            fwrite(&curCnt, sizeof(long long), 1, f);
            all += curCnt;
            pl = cur.l;
            pr = cur.r;
            curCnt = 1;
        }
        if (cur.readNext()) {
            st.insert(cur);
        }
    }
    fwrite(&pl, sizeof(int), 1, f);
    fwrite(&pr, sizeof(int), 1, f);
    fwrite(&curCnt, sizeof(long long), 1, f);
    fclose(f);
    printf("all = %lld\n", all);
}