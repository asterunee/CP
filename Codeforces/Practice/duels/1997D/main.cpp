#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        int N; 
        read(N); 
        vector<ll> A(N); 
        read(A); 
        vector<vector<int>> g(N); 
        REP(i, 1, N) {
            int p; 
            read(p); 
            g[--p].push_back(i); 
        }
        vector<int> ord{0}; 
        for (int i = 0; i < (int)ord.size(); ++i) {
            int v = ord[i]; 
            for (int u : g[v]) {
                ord.push_back(u); 
            }
        }
        vector<ll> dp(N); 
        RREP(i, (int) ord.size()) {
            int v = ord[i]; 
            if (g[v].empty()) {
                dp[v] = A[v]; 
                continue;
            } 
            ll mn = LLONG_MAX; 
            for (int u : g[v]) {
                chmin(mn, dp[u]); 
            }
            if (v) {
                dp[v] = A[v] < mn ? (A[v] + mn) / 2 : mn; 
            }
        }
        ll mn = LLONG_MAX; 
        for (int u : g[0]) {
            chmin(mn, dp[u]); 
        }
        print(A[0] + mn); 
    }
} 