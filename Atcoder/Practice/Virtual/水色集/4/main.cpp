#include "library/template.hpp"

using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int N; 
    read(N); 
    vector<int> id(N + 1); 
    vector<vector<int>> same(N + 1); 
    vector<vector<pair<int, int>>> ch (N + 1); 
    unordered_map<ull, int> mp; 
    mp.reserve(2 * N); 
    int sz = 1; 
    REP(i, 1, N + 1) {
        int x, y; 
        read(x, y); 
        int p = id[x]; 
        ull key = (ull(p) << 32) | ull(y); 
        auto it = mp.find(key); 
        int v; 
        if (it == mp.end()) {
            v = sz++; 
            mp.emplace(key, v); 
            ch[p].push_back({y, v}); 
        } else {
            v = it -> second; 
        }
        id[i] = v; 
        same[v].push_back(i); 
    }
    REP(v, sz) {
        sort(ALL(ch[v])); 
    }
    vector<int> ans; 
    ans.reserve(N); 
    vector<int> st{0}; 
    while (not st.empty()) {
        int u = st.back(); 
        st.pop_back(); 
        FOR(i, same[u]) {
            ans.push_back(i); 
        }
        for (int i = (int) ch[u].size() - 1; i >= 0; --i) {
            st.push_back(ch[u][i].second); 
        }
    }
    print(ans); 
}