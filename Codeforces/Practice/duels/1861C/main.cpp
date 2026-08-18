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
        string S; 
        read(S); 
        int n = 0, ok = 0; 
        int bad= 1e9; 
        bool ans = true; 
        for (char c : S) {
            if (c == '+') {
                ++n ; 
            } else if (c == '-') {
                if (bad == n) {
                    bad =1e9; 
                }
                --n; 
                chmin(ok, n); 
            } else if (c == '1') {
                if (bad <= n) {
                    ans = false; 
                }
                ok = n; 
            } else {
                if (n < 2 or ok >= n) {
                    ans = false; 
                }
                chmin(bad, n); 
            }
        }
        print(ans ? "YES" : "NO"); 
    }
} 