#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
	vector<int> ar;
	FILE *f = fopen("col_stat", "rb");
	int v;
	while (fread(&v, sizeof(int), 1, f)) {
		ar.push_back(-v);
		for (int i = 0; i < 2; ++i) {
			fread(&v, sizeof(int), 1, f);
		}
	}
	sort(ar.begin(), ar.end());
	for (int i = 0; i < ar.size(); ++i) {
		printf("%d %d\n", i + 1, -ar[i]);
	}
}