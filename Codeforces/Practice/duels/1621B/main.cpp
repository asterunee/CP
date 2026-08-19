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
        ll L = 1e18, R = -1; 
        ll cl = 1e18, cr = 1e18, cb = 1e18; 
        LOOP(N) {
            ll l, r, c; 
            read(l, r, c); 
            if (l < L) {
                L = l; 
                cl = c; 
                cb = 1e18; 
            } else if (l == L) {
                chmin(cl, c); 
            }
            if (r > R) {
                R = r; 
                cr = c; 
                cb = 1e18; 
            } else if (r == R) {
                chmin(cr, c); 
            }
            if (l == L and r == R) {
                chmin(cb, c); 
            }
            print(min(cl + cr, cb)); 
        }
    }
} 