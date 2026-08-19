#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"

using namespace std;
using namespace suisen;

using ll = long long;

signed main() {
    int T;
    read(T);

    LOOP(T) {
        int N, Q;
        read(N, Q);

        vector<ll> A(N);
        read(A);

        vector<pair<int, ll>> qs(Q);
        vector<ll> xs;
        xs.reserve(N + 2 * Q);

        REP(i, N - 1) {
            xs.push_back(A[i + 1] - A[i]);
        }

        vector<ll> cur = A;

        REP(q, Q) {
            int i;
            ll x;
            read(i, x);
            --i;

            qs[q] = { i, x };
            cur[i] = x;

            if (i) {
                xs.push_back(cur[i] - cur[i - 1]);
            }
            if (i + 1 < N) {
                xs.push_back(cur[i + 1] - cur[i]);
            }
        }

        sort(ALL(xs));
        xs.erase(unique(ALL(xs)), xs.end());

        int M = xs.size();

        FenwickTree<ll> cnt(M), sum(M);

        ll tot = 0;
        ll h = 0;

        auto add = [&](ll x) {
            int p = lower_bound(ALL(xs), x) - xs.begin();

            ll c = cnt.sum(0, p + 1);
            ll s = sum.sum(0, p + 1);

            h += (c + 1) * x + (tot - s);

            cnt.add(p, 1);
            sum.add(p, x);
            tot += x;
        };

        auto erase = [&](ll x) {
            int p = lower_bound(ALL(xs), x) - xs.begin();

            ll c = cnt.sum(0, p + 1);
            ll s = sum.sum(0, p + 1);

            h -= c * x + (tot - s);

            cnt.add(p, -1);
            sum.add(p, -x);
            tot -= x;
        };

        REP(i, N - 1) {
            add(A[i + 1] - A[i]);
        }

        for (auto [i, x] : qs) {
            if (i) {
                erase(A[i] - A[i - 1]);
            }
            if (i + 1 < N) {
                erase(A[i + 1] - A[i]);
            }

            A[i] = x;

            if (i) {
                add(A[i] - A[i - 1]);
            }
            if (i + 1 < N) {
                add(A[i + 1] - A[i]);
            }

            print(1LL * N * A[N - 1] - h);
        }
    }
}