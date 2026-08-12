#include "library/template.hpp"
#include "library/number/modint.hpp"
#include "library/math/factorial.hpp"

using namespace std; 
using namespace suisen; 
using mint = modint1000000007; 

signed main() {
    factorial<mint> fac(200000); 
    int T; 
    read(T); 
    while (T--) {
        int N;
        read(N); 
        vector<int> A(N + 1); 
        REP(i, 1, N + 1) {
            read(A[i]); 
        }
        if (N == 1) {
            print(0); 
            continue;
        }
        int ans = 0; 
        int cur = 1, nxt = 1; 
        bool ok = true; 
        REP(i, 1, N) {
            chmax(nxt, min(N, i + A[i])); 
            if (i == cur) {
                if (nxt == cur) {
                    ok = false; 
                    break; 
                }
                ++ans; 
                cur = nxt; 
                if (cur == N) {
                    break; 
                }

            }
        }
        print(ok ? ans : -1); 
    }
}