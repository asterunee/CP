#include "library/template.hpp"
#include "atcoder/maxflow"
using namespace std; 
using namespace suisen; 
using atcoder::mf_graph; 
using ll = long long;
signed main() {
    int N, M; 
    ll A, B, C; 
    read(N, M, A, B, C); 
    vector<string> S(N); 
    read(S); 
    int V = 4 * N * M; 
    int src= V, sink = V + 1; 
    mf_graph<ll> g(V + 2); 
    const ll INF = 1LL << 60; 
    auto id = [&](int i, int j, int k) {if (i < 0 or j < 0) {return src; } if (i >= N or j >= M) {return sink; } return k * N * M + i * M  + j; };
    auto add = [&](int u, int v, ll c) {g.add_edge(u, v, c); }; 
    REP(i, N) {
        REP(j, M) {
            int hb = id(i, j, 0); 
            int nhw = id(i, j, 1); 
            int vw = id(i, j, 2); 
            int nvb = id(i, j, 3); 
            add(src, hb, A); 
            add(id(i, j - 1, 0), hb, B); 
            add(nhw, sink, A); 
            add(nhw, id(i, j + 1, 1), B); 
            add(src, vw, A); 
            add(id(i - 1, j, 2), vw,  B); 
            add(nvb, sink, A); 
            add(nvb, id(i + 1, j, 3), B); 
            if (S[i][j] == '#') {
                add(hb, nvb, C); 
                add(src, vw, INF); 
                add(nhw, sink, INF); 
            } else {
                add(vw, hb, C); 
                add(nvb, nhw, C); 
                add(nvb, hb, INF); 
            }
        }
        
    }
    print(g.flow(src, sink)); 

}