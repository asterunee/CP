#include "library/template.hpp"
#include "library/graph/manhattan_mst.hpp"
#include "library/datastructure/union_find/union_find.hpp"

using namespace std; 
using namespace suisen; 
using ull = unsigned long long; 
using ll = long long; 
struct Plan {
    int x; 
    vector<int> path; 
}; 
signed main() {
    ios::sync_with_stdio(false); 
    cin.tie(NULL); 
    
    int T; 
    read(T); 
    while (T--) {
        int M, N; 
        read(M, N); 
        vector<vector<int>> g(M); 
        REP(i, M - 1) {
            int a, b; 
            read(a, b); 
            --a, --b; 
            g[a].push_back(b); 
            g[b].push_back(a); 
        }
        vector<int> par(M), dep(M); 
        par[0] = 0; 
        vector<int> st{ 0}; 
        while (not st.empty()) {
            int u = st.back(); 
            st.pop_back(); 
            for (int v : g[u]) {
                if (v == par[u]) {
                    continue; 
                }
                par[v] = u; 
                dep[v] = dep[u] + 1; 
                st.push_back(v); 
            }
        }
        auto get_path = [&](int u, int v) {
            vector<int> a, b; 
            while (dep[u] > dep[v]) {
                a.push_back(u); 
                u = par[u]; 
            }
            while (dep[v] > dep[u]) {
                b.push_back(v); 
                v = par[v]; 
            }
            while (u !=  v) {
                a.push_back(u); 
                b.push_back(v); 
                u = par[u]; 
                v = par[v]; 
            }
            a.push_back(u); 
            reverse(ALL(b)); 
            a.insert(a.end(), ALL(b)); 
            return a; 
        }; 
        vector<array<Plan, 2>> p(N); 
        REP(i, N) {
            REP(b, 2) {
                int x, u, v; 
                read(x, u, v); 
                --x, --u, --v; 
                p[i][b].x = x; 
                p[i][b].path = get_path(u, v); 
            }
        }
        vector<unsigned char> alive(M, true); 
        vector<vector<int>> contain(M); 
        vector<int> cnt; 
        string ans; 
        auto can = [&](const Plan&z) {
            int x = z.x; 
            if (not alive[x]) {
                return false; 
            }
            for (int id : contain[x]) {
                if (cnt[id] == 1) {
                    return false; 
                }
            }
            int c = 0; 
            for (int v : z.path) {
                if (alive[v] && v != x) {
                    ++c; 
                }
            }
            return c > 0; 
        };
        auto apply = [&](const Plan& z) {
    int x = z.x; 
    alive[x] = false; 

    for (int id : contain[x]) {
        --cnt[id]; 
    }

    int id = cnt.size(); 
    int c = 0; 

    for (int v : z.path) {
        if (alive[v]) {
            ++c; 
        }
    }

    cnt.push_back(c); 

    for (int v : z.path) {
        contain[v].push_back(id); 
    }
};
        int fail = -1; 
        REP(i, N) {
            if (can(p[i][0])) {
                ans += '0'; 
                apply(p[i][0]); 
            } else if (can(p[i][1])) {
                ans += '1'; 
                apply(p[i][1]); 
            } else {
                fail = i; 
                break; 
            }
        }
        if (fail == -1) {
            print("YES"); 
            print(ans); 
        } else {
            print("NO", fail + 1); 
            if (ans.empty()) {
                print("-"); 
            } else {
                print(ans); 
            }
        }
    }
}