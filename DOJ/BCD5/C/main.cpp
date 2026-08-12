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
        int N, l, r, k; 
        read(N, l, r, k); 
        int m = r - l + 1; 
        if (k < m) {
            print(0); 
            continue; 
        }
        mint ans = mint(m) * fac.perm(k - 1, m - 1) * fac.fac(N- m); 
        print(ans); 
    }
}