#include "library/template.hpp"
using namespace std; 
using namespace suisen; 

signed main() {
    int N; 
    read(N); 
    vector<vector<int>> g(N); 
    vector<int> deg(N), xr(N); 
    REP(i, N) {
        int C; read(C); 
        deg[i] = C; 
        LOOP(C) {
            int x; 
            read(x); 
            --x; 
            g[x].push_back(i); 
            xr[i] ^= x; 
        }
    }
    queue<int> q; 
    REP(i, N) {
        if (deg[i] == 1) {
            q.push(i); 
        }
    }
    vector<char> used(N), gone(N); 
    vector<int> ans; 
    ans.reserve(N); 
    while (!q.empty()) {
        int v = q.front(); 
        q.pop(); 
        if (used[v] or deg[v] != 1) {
            continue; 
        }
        int x = xr[v]; 
        if (gone[x]) {
            continue;
        }
        used[v] = true; 
        gone[x] = true; 
        ans.push_back(v); 
        for (int u : g[x]) {
            if (used[u]) {
                continue; 
            }
            --deg[u]; 
            xr[u] ^= x; 
            if (deg[u] ==1) {
                q.push(u); 
            }
        }
    }
    if ((int) ans.size() != N) {
        print(-1); 
        return 0; 
    }
    REP(i, N) {
        if (i) {
            cout << ' '; 
        }
        cout << ans[i] + 1; 
    }
    cout << '\n'; 
}