#include "library/template.hpp"

using namespace std;
using namespace suisen;
using ll = long long;
constexpr ll MOD = 1000000007;
signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    read(T);

    while (T--) {
        ll N;
        read(N);
        ll dp[2][2][2]{};
        dp[0][0][1] = 1;
        RREP(j, 1, 62) {
            ll ndp[2][2][2]{};
            int nb = N >> j & 1;
            REP(t, 2) {
                REP(st, 2) {
                    REP(tight, 2) {
                        ll cur = dp[t][st][tight];
                        if (!cur) {
                            continue;
                        }
                        REP(nt, 2) {
                            if (t and nt) {
                                continue;
                            }
                            REP(q, 2) {
                                if (!nt and q) {
                                    continue;
                                }
                                if (nt and !st and !q) {
                                    continue;
                                }
                                int cb = t | q;
                                if (tight and cb > nb) {
                                    continue;
                                }
                                int ns = st | nt;
                                int ntight = tight and cb == nb;
                                ndp[nt][ns][ntight] += cur;
                                ndp[nt][ns][ntight] %= MOD;
                            }
                        }
                    }
                }
            }
            memcpy(dp, ndp, sizeof dp);
        }
        ll ans = 0;
        int nb = N & 1;
        REP(t, 2) {
            REP(tight, 2) {
                if (tight and t > nb) {
                    continue;
                }
                ans += dp[t][1][tight];
                ans %= MOD;
            }
        }
        print(ans);
    }
}