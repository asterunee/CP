#include "library/template.hpp"
#include "library/number/modint.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using mint = modint998244353; 
signed main() {
    int N, M; 
    read(N, M); 
    vector<string> S(N); 
    read(S); 
    vector<vector<mint>> nxt(N + 1, vector<mint>(N + 1)); 
    REP(i, N) {
        nxt[i][i + 1] = 1; 
    }
    RREP(p, M) {
        vector<vector<mint>> dp(N + 1, vector<mint>(N + 1)); 
        REP(l, N) {
            vector<mint> f(N + 1); 
            f[l] = 1; 
            REP(d, 10) {
                vector<mint> nf = f; 
                REP(i, l, N) {
                    if (f[i] == mint(0)) {
                        continue;
                    }
                    REP(j, i + 1, N + 1) {
                        char c = S[j - 1][p]; 
                        if (c != '?' && c - '0' != d) {
                            break; 
                        }
                        nf[j] += f[i] * nxt[i][j]; 
                    }
                }
                f.swap(nf); 
            }
            REP(r, l + 1, N + 1) {
                dp[l][r] = f[r]; 
            }
        }
        nxt.swap(dp); 
    }
    print(nxt[0][N].val()); 
}