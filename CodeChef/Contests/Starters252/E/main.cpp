#include "library/template.hpp"

using namespace std;
using namespace suisen;

using ll = long long;

constexpr ll MOD = 998244353;

signed main() {
    int T;
    read(T);

    LOOP(T) {
        int N;
        read(N);

        vector<int> cnt(N + 3);

        LOOP(N) {
            int x;
            read(x);
            ++cnt[x];
        }

        vector<ll> w(N + 3);

        REP(i, N + 3) {
            ll p = 1;

            LOOP(cnt[i]) {
                p = p * 2 % MOD;
            }

            w[i] = (p - 1 + MOD) % MOD;
        }

        ll ans = 0;
        ll pre = 1;

        REP(m, N + 2) {
            if (m) {
                ans += pre;
            }

            ans += pre * w[m + 1] % MOD;
            ans %= MOD;

            pre = pre * w[m] % MOD;
        }

        print(ans);
    }
}