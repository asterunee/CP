#include "library/template.hpp"

using namespace std;
using namespace suisen;

using ll = long long;

constexpr int MOD = 998244353;
constexpr int INV2 = 499122177;

int C[405][405];

ll pw(ll a, int n) {
    ll r = 1;

    while (n) {
        if (n & 1) r = r * a % MOD;
        a = a * a % MOD;
        n >>= 1;
    }

    return r;
}

signed main() {
    C[0][0] = 1;

    REP(i, 1, 401) {
        C[i][0] = C[i][i] = 1;

        REP(j, 1, i) {
            C[i][j] = C[i - 1][j - 1] + C[i - 1][j];

            if (C[i][j] >= MOD) {
                C[i][j] -= MOD;
            }
        }
    }

    int T;
    read(T);

    LOOP(T) {
        int N;
        read(N);

        vector<int> A(N);
        read(A);

        int z = 0, o = 0;
        vector<int> B;

        for (int x : A) {
            if (x == 0) {
                ++z;
            } else if (x == 1) {
                ++o;
            } else {
                B.push_back(x - 1);
            }
        }

        int p = B.size();

        vector dp(p + 1, vector<int>(z + 1));
        dp[0][0] = 1;

        int used = 0;

        for (int b : B) {
            for (int k = used; k >= 0; --k) {
                for (int e = z - b; e >= 0; --e) {
                    if (!dp[k][e]) {
                        continue;
                    }

                    int &v = dp[k + 1][e + b];
                    v += dp[k][e];

                    if (v >= MOD) {
                        v -= MOD;
                    }
                }
            }

            ++used;
        }

        vector<int> f(N + 1);

        REP(k, p + 1) {
            REP(e, z + 1) {
                if (!dp[k][e]) {
                    continue;
                }

                int s = k + e;

                f[s] = (
                    f[s] +
                    (ll) dp[k][e] * C[z][e]
                ) % MOD;
            }
        }

        vector<int> cnt(N + 1);

        REP(s, N + 1) {
            if (!f[s]) {
                continue;
            }

            REP(q, o + 1) {
                if (s + q > N) {
                    break;
                }

                cnt[s + q] = (
                    cnt[s + q] +
                    (ll) f[s] * C[o][q]
                ) % MOD;
            }
        }

        ll ans = 0;

        REP(s, 1, N) {
            int t = N - s;

            ll trees =
                pw(s, t - 1) *
                pw(t, s - 1) % MOD;

            ans += (ll) cnt[s] * trees % MOD;

            if (ans >= MOD) {
                ans -= MOD;
            }
        }

        print(ans * INV2 % MOD);
    }
}