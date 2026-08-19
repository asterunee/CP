#include "library/template.hpp"

using namespace std;
using namespace suisen;

using ll = long long;

signed main() {
    int T;
    read(T);

    LOOP(T) {
        int N;
        read(N);

        vector<ll> A(N);
        read(A);

        vector<bool> pre(N + 1), suf(N + 1);
        pre[0] = suf[N] = true;

        ll x = 0;

        REP(i, N) {
            x = A[i] - x;
            pre[i + 1] = pre[i] and x >= 0;
        }

        x = 0;

        RREP(i, N) {
            x = A[i] - x;
            suf[i] = suf[i + 1] and x >= 0;
        }

        int ans = 0;

        REP(i, N) {
            ans += pre[i] and suf[i + 1];
        }

        print(ans);
    }
}