#include "library/template.hpp"
#include "library/datastructure/union_find/union_find.hpp"

using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int N; 
    read(N); 
    vector<ll> L(N), R(N); 
    vector<ll>xs; 
    xs.reserve(2 * N); 
    REP(i, N) {
        ll X, r; 
        read(X, r); 
        L[i]  = X - r; 
        R[i] = X  + r; 
        xs.push_back(L[i]); 
        xs.push_back(R[i]); 
    }
    sort(ALL(xs)); 
    xs.erase(unique(ALL(xs)), xs.end()); 
    int V = xs.size(); 
    UnionFind uf(V); 
    vector<pair<int, int>> e(N); 
    REP(i, N) {
        int l = lower_bound(ALL(xs), L[i]) - xs.begin(); 
        int r = lower_bound(ALL(xs), R[i]) - xs.begin(); 
        e[i] = {l, r}; 
        uf.merge(l, r); 
    }
    vector<int> ec(V); 
    FOR(p, e) {
        ++ec[uf.root(p.first)]; 
    }
    ll ans = 0; 
    REP(v, V) {
        if (uf.root(v) != v) {
            continue; 
        }
        ans += min(uf.size(v), ec[v]); 

    }
    print(ans); 
}