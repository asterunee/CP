#include "library/template.hpp"

using namespace std;
using namespace suisen;

using ll = long long;

signed main() {
    int T;
    read(T);

    LOOP(T) {
        int N;
        ll K;
        read(N, K);

        vector<ll> A(N);
        read(A);

        ll sum = 0, mx = 0;
        int ans = 0;

        REP(i, N) {
            sum += A[i];
            chmax(mx, A[i]);

            if (sum - mx <= K) {
                ans = i + 1;
            }
        }

        print(ans);
    }
}