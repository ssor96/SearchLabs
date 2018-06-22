#include <cstdio>
#include <set>
#include <vector>
#include <cmath>

using namespace std;
using ll = long long;
vector<ll> c, fc, sc;
ll numberOfToks = 0;

int main() {
	FILE *f = fopen("col_stat");
	c.push_back(0);
	fc.push_back(0);
	sc.push_back(0);
	int vc, vfc, vsc;
	while (fread(&vc, sizeof(int), 1, f) == 1) {
		fread(&vfc, sizeof(int), 1, f);
		fread(&vsc, sizeof(int), 1, f)
		c.push_back(vc);
		fc.push_back(vfc);
		sc.push_back(vsc);
		numberOfToks++;
	}
	int id1, id2;
	ll pc;
	while (scanf("%d %d %lld", &id1, &id2, &pc)) {
	    double mean = pc / double(bigramCount);
	    double distMean = fc[id1] * sc[id2] / double(bigramCount) / double(bigramCount)
	    double t = (mean - distMean) / (sqrt(mean/double(bigramCount)))

	    if (t > 2.576) {
	        //push
	    }

	    double o11 = pc;
	    double o12 = sc[id2] - o11;
	    double o21 = fc[id1] - o11
	    double o22 = bigramCount - fc[id1] - sc[id2];

	    double chi = bigramCount * (o11 * o22 - o12 * o21) * (o11 * o22 - o12 * o21) / double((o11 + o12) * (o11 + o21) * (o12 + o22) * (o21 + o22));
	    if (chi > 3.841) {
	        if (o11 > 10 && o12 > 10 && o21 > 10 && o22 > 10) {
	            //push
	    	}
	    }
	}
}