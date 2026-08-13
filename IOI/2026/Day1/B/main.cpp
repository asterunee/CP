#include "library/template.hpp"
#include "monuments.h"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
constexpr ll INF = (ll) 1e16; 
constexpr int RED = 0; 
constexpr int BLUE = 1; 

struct Monument {
    int x, c; 
    Monument() : x(0), c(0) {}
    Monument(int x, int c) : x(x), c(c) {}
}; 
struct Query {
    int x, y, k; 
    Query() : x(0), y(0), k(0) {}
    Query(int x, int y, int k) : x(x), y(y), k(k) {}
}; 
vector<ll> process_queries(
    vector<int> red, 
    vector<int> blue, 
    vector<Query> qs
){
    int r = red.size(); 
    int b = blue.size(); 
    int q = qs.size(); 
    vector<ll> vr(r), vb(b); 
    REP(i, r) {
        vr[i] = red[i]; 
    }
    REP(i, b) {
        vb[i] = -ll(blue[i]); 
    }
    FenwickTree<ll> fr(vr), fb(vb); 
    vector<pair<int, int>> dr; 
    for (int i = 0, j = 0; i < r; ++i) {
        while (j < b && blue[j] <= red[i]) {
            ++j; 
        }
        if (j < b) {
            dr.emplace_back(j - i, i); 
        }
    }
    vector<pair<int, int>> db; 
    for (int i = r - 1, j = b - 1; j >= 0; --j) {
        while (i >= 0 && red[i] >= blue[j]) {
            --i; 
        }
        db.emplace_back(j - i, j); 
    }
    vector<int> ord(q); 
    iota(ALL(ord), 0);
    sort(ALL(ord), [&](int i, int j) { return qs[i].y - qs[i].x < qs[j].y - qs[j].x; });  
    sort(ALL(dr));
    sort(ALL(db));
    vector<ll> ans(q); 
    int rp = 0, bp = 0; 
    for (int id : ord) {
        auto [x, y, k] = qs[id]; 
        if (k == 0) {
            continue; 
        }
        int d = y - x; 
        while (rp < int(dr.size()) && dr[rp].first <= d) {
            auto [_, i] = dr[rp++]; 
            fr.add(i, -2LL * red[i]); 
        }
        while (bp < int(db.size()) && db[bp].first <= d) {
            auto [_, j] = db[bp++]; 
            fb.add(j, 2LL * blue[j]); 
        }

    }
}