#include "library/template.hpp"
#include "library/graph/manhattan_mst.hpp"
#include "library/datastructure/union_find/union_find.hpp"

using namespace std; 
using namespace suisen; 
using ull = unsigned long long; 
using ll = long long; 

signed main() {
    ios::sync_with_stdio(false); 
    cin.tie(NULL); 
    
    int T; 
    read(T); 
    while (T--) {
        int N; 
        read(N); 
        vector<pair<ll, ll>> P(N); 
        for (auto& [x, y] : P) {
            read(x, y); 
        }
        if(N == 1) {
            print(0); 
            continue; 
        }
        ll ans = numeric_limits<ll>::max(); 
        REP(skip, N) {
            vector<pair<ll, ll>> Q; 
            Q.reserve(N - 1); 
            REP(i, N) {
                if (i != skip) {
                    Q.push_back(P[i]); 
                }
            }
            auto mst = manhattan_mst<ll> (Q); 
            const auto& tr = mst.get_mst(); 
            int M = N - 1; 
            vector<pair<ll, int>> star(M); 
            REP(ix, M) {
                ll x = Q[ix].first; 
                REP(iy, M) {
                    ll y = Q[iy].second; 
                    REP(v, M) {
                        auto [px, py] = Q[v]; 
                        star[v] = {abs(x - px) + abs(y- py), v}; 
                    }
                    sort(ALL(star)); 
                    UnionFind uf(N); 
                    int a = 0; 
                    int b = 0; 
                    int used = 0; 
                    ll sum = 0; 
                    while (used < N - 1) {
                        bool take_tree = b == M; 
                        if (!take_tree && a < int(tr.size())) {
                            take_tree = get<2> (tr[a]) <= star[b].first; 
                        }
                        if (take_tree) {
                            auto [u, v, w] = tr[a++]; 
                            if (uf.merge(u, v)) {
                                sum += w; 
                                ++used; 
                            }
                        } else {
                            auto [w, v] = star[b++]; 
                            if (uf.merge(M, v)) {
                                sum += w; 
                                ++used; 
                            }
                        }
                    }
                    chmin(ans, sum); 
                }
            }
        }
        print(ans); 
    }
}