#include "library/template.hpp"
#include "library/number/modint.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using mint = modint1000000007; 
signed main() {
    int N, M, K; 
    read(N, M, K); 
    int S = N * M; 
    vector<mint> fac(S + 1), ifac(S + 1); 
    fac[0] = 1; 
    REP(i, 1, S + 1) {
        fac[i] = fac[i - 1] * i; 
    }
    ifac[S] = fac[S].inv(); 
    RREP(i, S) {
        ifac[i]= ifac[i + 1] * (i + 1); 
    }
    auto C = [&](int n, int r) -> mint {if (r < 0 || r > n) return 0; return fac[n] * ifac[r] * ifac[n - r]; };
    mint sum = 0; 
    REP(d, 1, N) {
        sum += mint(d) * (N - d) * M  * M; 
    } 
    REP(d, 1, M) {
        sum += mint(d) * (M - d) * N * N; 
    } 
    mint ans = sum * C(S - 2, K - 2); 
    print(ans.val()); 
}