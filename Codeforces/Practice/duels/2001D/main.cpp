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
        vector<int> A(N), last(N + 1, -1); 
        vector<vector<int>> pos(N + 1); 
        REP(i, N) {
            read(A[i]); 
            last[A[i]] = i; 
            pos[A[i]].push_back(i); 
        }
        set<pair<int, int>> lim; 
        REP(x, 1, N + 1) {
            if (last[x] != -1) {
                lim.emplace(last[x], x); 
            }
        }
        set<pair<int, int>> cur; 
        vector<char> used(N + 1); 
        vector<int> ans; 
        int l = 0, rr = -1; 
        while (!lim.empty()) {
            int r = lim.begin() -> first; 
            while (rr < r) {
                ++rr; 
                if (!used[A[rr]]) {
                    cur.emplace(A[rr], rr); 
                }
            }
            int v, p; 
            if (ans.size()  & 1) {
                tie(v, p) = *cur.begin(); 
            } else {
                v = prev(cur.end()) -> first; 
                p = cur.lower_bound({v, -1}) -> second; 
            }
            ans.push_back(v); 
            used[v] = true; 
            lim.erase({last[v], v}); 
            REP(i, l, p + 1) {
                cur.erase({A[i], i}); 
            }
            for (int i : pos[v]) {
                if (i <= rr) {
                    cur.erase({v, i}); 
                }
            }
            l = p + 1; 
        }
        print(ans.size()); 
        print(ans); 
    }
    
} 
