#include <iostream>
#include <fstream>
#include <cstdio>
#include <queue>
#include <vector>
#include <cmath>

using namespace std;

using ll = long long;
vector<string> toks;
vector<ll> c, fc, sc;
ll numberOfToks = 0;
double bigramCount = 500944026;

int main() {
    ifstream is("stat/pairs_dict.txt");
    toks.push_back("");
    string s;
    while (getline(is, s)) {
        toks.push_back(s);
    }
    priority_queue<pair<double, pair<int, int>>> one;
    priority_queue<pair<double, pair<int, int>>> two;
    FILE *f = fopen("stat/col_stat", "rb");
    c.push_back(0);
    fc.push_back(0);
    sc.push_back(0);
    int vc, vfc, vsc;
    while (fread(&vc, sizeof(int), 1, f) == 1) {
        fread(&vfc, sizeof(int), 1, f);
        fread(&vsc, sizeof(int), 1, f);
        c.push_back(vc);
        fc.push_back(vfc);
        sc.push_back(vsc);
        numberOfToks++;
    }
    fclose(f);
    int id1, id2;
    ll pc;
    f = fopen("stat/summed_col", "rb");
    while (fread(&id1, sizeof(int), 1, f) == 1) {
        fread(&id2, sizeof(int), 1, f);
        fread(&pc, sizeof(ll), 1, f);
        if (c[id1] > 1e+7 || c[id2] > 1e+7) continue;
        double mean = pc / bigramCount;
        double distMean = fc[id1] * sc[id2] / bigramCount / bigramCount;
        double t = (mean - distMean) / (sqrt(mean/bigramCount));

        if (t > 2.576) {
            one.push({t, {id1, id2}});
        }

        double o11 = pc;
        double o12 = sc[id2] - o11;
        double o21 = fc[id1] - o11;
        double o22 = bigramCount - fc[id1] - sc[id2];

        double chi = bigramCount * (o11 * o22 - o12 * o21) * (o11 * o22 - o12 * o21) / double((o11 + o12) * (o11 + o21) * (o12 + o22) * (o21 + o22));
        if (chi > 3.841) {
            if (o11 > 10 && o12 > 10 && o21 > 10) {
                two.push({chi, {id1, id2}});
            }
        }
    }
    fclose(f);
    cout << one.size() << " " << two.size() << endl;
    for (int i = 0; i < 100; ++i) {
        int id1 = one.top().second.first;
        int id2 = one.top().second.second;
        double t = one.top().first;
        one.pop();
        cout << toks[id1] << " " << toks[id2] << " | " 
             << c[id1] << " " << c[id2] << " " << t << endl;
    }
    cout << endl;
    for (int i = 0; i < 100; ++i) {
        int id1 = two.top().second.first;
        int id2 = two.top().second.second;
        double t = two.top().first;
        two.pop();
        cout << toks[id1] << " " << toks[id2] << " | " 
             << c[id1] << " " << c[id2] << " " << t << endl;
    }
}