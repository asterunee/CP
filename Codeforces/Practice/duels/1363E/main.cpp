#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int N; 
    read(N); 
    vector<ll> A(N); 
    vector<int> B(N), C(N); 
    REP(i, N) {
        read(A[i], B[i], C[i]); 
    }
    vector<vector<int>> g(N); 
    REP(i, N - 1) {
        int u, v; 
        read(u, v); 
        --u, --v; 
        g[u].push_back(v); 
        g[v].push_back(u); 
    
    }
    vector<int> par(N , - 1), ord = {0}; 
    vector<ll> mn(N); 
    mn[0] = A[0]; 
    for (int i= 0; i < (int)ord.size(); ++i) {
        int u = ord[i]; 
        for (int v : g[u]) {
            if (v == par[u]) {
                continue; 
            }
            par[v] = u; 
            mn[v] = min(mn[u], A[v]); 
            ord.push_back(v); 
        }
    }
    vector<int> x(N), y(N); 
    ll ans = 0; 
    RREP(ii, N) {
        int u = ord[ii]; 
        if (B[u] != C[u]) {
            if (B[u] == 0) {
                ++x[u]; 
            } else {
                ++y[u]; 
            }
            
        }
        for (int v : g[u]) {
            if (par[v] == u) {
                x[u] += x[v]; 
                y[u] += y[v]; 
            }
        }
        int k = min(x[u], y[u]); 
        ans += 2LL * k * mn[u]; 
        x[u] -= k; 
        y[u] -= k; 
    }
    if (x[0] or y[0]) {
        print(-1); 
    } else {
        print(ans); 
    }
} 
