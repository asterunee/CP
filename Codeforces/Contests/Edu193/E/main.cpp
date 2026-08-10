#include "library/template.hpp"
#include "library/graph/csr_graph.hpp"
#include "library/convolution/convolution_large.hpp"
using namespace std;
using namespace suisen;
using ll = long long; 
using mint = atcoder::modint998244353;
signed main() {
    int T; 
    read(T); 
    while (T--) {
        int N; 
        read(N); 
        vector<pair<int, int>> E(N - 1); 
        REP(i, N - 1) {
            auto &[u, v] = E[i]; 
            read(u, v); 
            --u, --v; 
        }
        auto G =Graph<>::create_undirected_graph(N, E); 
        vector<int> par(N), dep(N), ord; 
        auto farthest = [&](int s) {
            ord.clear(); 
            ord.push_back(s); 
            par[s] = s; 
            dep[s] = 0; 
            int z = s; 
            REP(i, N) {
                int u = ord[i]; 
                if (dep[z] < dep[u]) {
                    z = u; 
                }
                for (int v : G[u]) {
                    if (v == par[u]) {
                        continue;
                    }
                    par[v] = u; 
                    dep[v] = dep[u] + 1; 
                    ord.push_back(v); 
                }
            }
            return z; 
        };
        int s = farthest(0); 
        int t = farthest(s); 
        int D = dep[t]; 
        int R = D / 2; 
        int c1 = t; 
        REP(_, R) {
            c1 = par[c1]; 
        }
        int c0 = par[c1]; 
        vector<int> side(N); 
        ord.clear(); 
        ord.push_back(c0); 
        ord.push_back(c1);
        par[c0] = c0; 
        par[c1] = c1; 
        dep[c0] = 0; 
        dep[c1] = 0; 
        side[c0] = 0; 
        side[c1] = 1; 
        REP(i, N) {
            int u = ord[i]; 
            for (int v : G[u]) {
                if ((u == c0 and  v == c1) or (u == c1 and v == c0)) {
                    continue; 
                }
                if (v == par[u]) {
                    continue; 
                }
                par[v] = u; 
                dep[v] = dep[u ] + 1; 
                side[v] = side[u]; 
                ord.push_back(v); 
            }
        } 
        vector<mint> A(R + 1), B(R + 1); 
        vector<unsigned char> has(N); 
        A[R] = 1; 
        B[R] = 1; 
        RREP(ii, N) {
            int u = ord[ii]; 
            if (dep[u] == R) {
                has[u] =1; 
                continue; 
            }
            int cnt = 0; 
            for (int v : G[u]) {
                if (par[v] == u and has[v]) {
                    ++cnt; 
                }
            }
            has[u] = cnt > 0; 
            if (cnt >= 2) {
                if (side[u] == 0) {
                    A[dep[u]] = 1; 
                } else {
                    B[dep[u]] = 1; 
                }
            }
        }
        auto C = convolution_large<mint> (A, B); 
        vector<int> ans; 
        REP(i, int(C.size())) {
            if (C[i].val()) {
                ans.push_back(i + 1); 
            }
        }
        print(ans.size(), ans); 
    }
}