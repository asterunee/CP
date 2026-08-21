#include "library/template.hpp"
#include "library/datastructure/union_find/union_find.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
struct Edge{int u, v,w; bool used = false; }; 
signed main() {
    int N, M; 
    read(N, M); 
    vector<Edge> es(M); 
    vector<vector<int>> g(N); 
    int X = 0; 
    REP(i, M) {
        int u, v, w; 
        read(u, v, w); 
        --u, --v; 
        es[i] = {u, v, w}; 
        g[u].push_back(v); 
        g[v].push_back(u); 
        X ^= w; 
    }
    UnionFind uf(N); 
    set<int> rem; 
    REP(i, N) {
        rem.insert(i); 
    }
    vector<int> mark(N); 
    int stamp = 0; 
    int cc = 0; 
    while (!rem.empty()) {
        ++cc; 
        int s = *rem.begin(); 
        rem.erase(rem.begin()); 
        queue<int> q; 
        q.push(s); 
        while (!q.empty()) {
            int v = q.front(); 
            q.pop(); 
            ++stamp; 
            for (int u : g[v]) {
                mark[u] = stamp; 
            }
            for (auto it = rem.begin(); it != rem.end(); ) {
                int u = *it; 
                if (mark[u] != stamp) {
                    uf.merge(v, u); 
                    q.push(u); 
                    it = rem.erase(it); 
                } else {
                    ++it; 
                }
            }
        }
    }
    ll missing = 1LL * N *( N - 1) / 2 - M; 
    vector<int> ord(M); 
    iota(ALL(ord), 0); 
    sort(ALL(ord), [&](int i, int j) {return es[i].w < es[j].w; }); 
    ll base = 0; 
    for (int i : ord) {
        auto &[u, v,w, used] = es[i]; 
        if (uf.merge(u, v)) {
            base += w; 
            used = true; 
        }
    }
    if (missing > N - cc) {
        print(base); 
        return 0; 
    }
    UnionFind fixed(N);
    for (auto [u, v, w, used] : es) {
        if (used) {
            fixed.merge(u, v); 
        }
    }
    ll rep = 1LL << 60; 
    for (auto [u, v, w, used] : es) {
        if (!used and !fixed.same(u, v)) {
            chmin(rep, (ll)w); 
        }
    }
    print(base + min<ll> (X, rep)); 
} 
