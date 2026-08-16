#include "library/template.hpp"
#include "library/number/modint.hpp"

using namespace std; 
using namespace suisen; 

using ll = long long; 
using mint = static_modint<998244353>; 

signed main() {
    int N, M; 
    read(N, M); 

    vector<int> cnt(N); 

    REP(_, M) {
        int a; 
        read(a); 
        ++cnt[a]; 
    }

    mint ans = 1; 

    for (int d = 1; d < N; d += 2) {
        vector<int> c; 

        for (ll x = d; x < N; x <<= 1) {
            c.push_back(cnt[x]); 
        }

        bool any = false; 
        FOR(x, c) {
            any |= x > 0; 
        }

        if (not any) {
            continue; 
        }

        int L = c.size(); 
        int P = int(1LL << L); 

        vector<char> dp(P), ndp(P); 
        dp[0] = 1; 

        REP(k, L) {
            if (c[k] == 0) {
                continue; 
            }

            int w = int(1LL << k); 
            int q = P / w; 

            fill(ALL(ndp), 0); 

            if (c[k] + 1 >= q) {
                REP(r, w) {
                    bool ok = false; 

                    REP(t, q) {
                        ok |= dp[r + t * w]; 
                    }

                    if (ok) {
                        REP(t, q) {
                            ndp[r + t * w] = 1; 
                        }
                    }
                }
            } else {
                REP(r, w) {
                    int sum = 0; 

                    REP(j, c[k] + 1) {
                        int t = (q - j) % q; 
                        sum += dp[r + t * w]; 
                    }

                    REP(t, q) {
                        ndp[r + t * w] = sum > 0; 

                        int add = (t + 1) % q; 
                        int del = (t - c[k] + q) % q; 

                        sum += dp[r + add * w]; 
                        sum -= dp[r + del * w]; 
                    }
                }
            }

            dp.swap(ndp); 
        }

        int ways = 0; 

        FOR(x, dp) {
            ways += x; 
        }

        ans *= ways; 
    }

    print(ans.val()); 
}