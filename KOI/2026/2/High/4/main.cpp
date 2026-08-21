#include "library/template.hpp"
#include "atcoder/maxflow"
using namespace std; 
using namespace suisen; 
using atcoder::mf_graph; 
using ll = long long; 
struct Info {int a, b, c, id; }; 

signed main() {
    int N, T; 
    read(N, T); 
    vector<vector<Info>> v(T + 2); 
    REP(i, N) {
        int a, b, c, d; 
        read(a, b, c, d); 
        v[d].push_back({a, b, c, i + 1}); 
    }
    REP(d, T + 2) {
        v[d].push_back({-1, 0, 0, -1}); 
        v[d].push_back({1001, 0, 0, -1}); 
        if (1 <= d and d <= T) {
            sort(ALL(v[d]), [](auto x, auto y) {return x.a < y.a; }); 
        }
    }
    vector<vector<int>> id(T + 2); 
    int V = 0; 
    REP(d, T + 2) {
        id[d].resize(v[d].size() - 1); 
        for (int & x : id[d]) {
            x = V++; 
        }
    }
    vector<ll> w(V); 
    vector<tuple<int, int, ll>> es; 
    REP(d,1, T + 1) {
        int n = v[d].size() - 2; 
        REP(j, 1, n) {
            w[id[d][j]] = v[d][j + 1].b - v[d][j].b; 
        }
        REP(j, 1, n + 1) {
            es.emplace_back(id[d][j - 1], id[d][j], v[d][j].c); 
        }
    }
    REP(d, 1, T + 2) {
        int x = 0, y = 0; 
        while (x + 1 < (int)v[d -  1].size() and y + 1 < (int) v[d].size()) {
            int l1 = v[d - 1][x].a; 
            int r1 = v[d - 1][x + 1].a; 
            int l2 = v[d][y].a; 
            int r2 = v[d][y + 1].a; 
            int len = min(r1, r2) - max(l1, l2); 
            if (len > 0) {
                es.emplace_back(id[d - 1][x], id[d][y], len); 
            }
            if (r1 == r2) {
                ++x, ++y; 
            } else if (r1 < r2) {
                ++x; 
            } else {
                ++y; 
            }
        }
    }
    int src = V, sink= V + 1; 
    mf_graph<ll> g(V + 2); 
    auto add_bi = [&](int u, int v, ll c) {g.add_edge(u, v, c); g.add_edge(v, u, c); };
    for (auto [u, v, c] : es) {
        add_bi(u, v, c); 
    } 
    constexpr ll INF = 1LL << 50; 
    g.add_edge(id[0][0], sink, INF); 
    g.add_edge(id[T + 1][0], sink, INF); 
    ll base = 0; 
    REP(d, 1, T+ 1) {
        int n = v[d].size() - 2; 
        g.add_edge(id[d][0], sink, INF); 
        g.add_edge(id[d][n], sink, INF); 
        REP(j, 1, n) {
            int u = id[d][j]; 
            if (w[u] >0) {
                base += w[u]; 
                g.add_edge(src, u, w[u]); 
            } else if (w[u] < 0) {
                g.add_edge(u, sink, -w[u]); 
            }
        }

    }
    ll flow = g.flow(src, sink); 
    auto cut = g.min_cut(src); 
    vector<int> ans; 
    REP(d, 1, T + 1) {
        int n = v[d].size() - 2; 
        REP(j, 1, n + 1) {
            if (cut[id[d][j - 1]] != cut[id[d][j]]) {
                ans.push_back(v[d][j].id); 
            }
        }
    }
    print(base - flow); 
    print(ans.size()); 
    REP(i, (int)ans.size()) {
        if (i) {
            cout << ' '; 
        } 
        cout << ans[i]; 
    }
    cout << '\n'; 
}