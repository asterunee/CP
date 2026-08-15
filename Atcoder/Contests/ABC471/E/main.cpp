#include "library/template.hpp"
#include "library/number/modint.hpp"
#include "library/math/factorial.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using mint = modint998244353; 
signed main() {
    int N, K; 
    read(N, K); 
    vector<mint> A(N); 
    REP(i, N) {
        ll x; 
        read(x); 
        A[i] = x; 
    }
    mint s = 0, sq = 0; 
    FOR(x, A) {
        s += x; 
        sq += x * x; 
    }
    factorial<mint> fac(N); 
    mint ans = fac.binom(N - 1, K - 1) * sq; 
    if (K >= 2) {
        ans += fac.binom(N - 2, K - 2) * (s * s - sq); 
    }
    print(ans.val()); 
}