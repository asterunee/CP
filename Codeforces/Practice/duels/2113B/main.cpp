#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(NULL); 
    int T; 
    read(T); 
    LOOP(T) {
        ll W, H, A, B, x1, y1, x2, y2; 
        read(W, H, A, B, x1, y1, x2, y2); 
        ll dx = abs(x1 - x2); 
        ll dy = abs(y1 - y2); 
        bool ok; 
        if (dx < A) {
            ok = dy % B == 0; 
        } else if (dy < B) {
            ok = dx % A == 0; 
        } else {
            ok = dx % A == 0 or dy % B == 0; 
        }
        print(ok ? "Yes" : "No"); 
    }
} 
