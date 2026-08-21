#include "library/template.hpp"

using namespace std;
using namespace suisen;
using ll = long long;
constexpr ll INF = (1LL << 60);
constexpr int K = 10;
using Mat = array<array<ll, K>, K>;
signed main() {
    int T;
    read(T);
    LOOP(T) {
        int N;
        ll M;
        read(N, M);
        vector<int> A(N), B(N);
        read(A);
        read(B);
        int L = 1;
        REP(i, N) {
            L = L / gcd(L, A[i]) * A[i];
        }
        auto safe = [&](ll x, int p) {
            int i = p == 0 ? N - 1 : p - 1;
            return x % A[i] != B[i];
        };
        auto empty_mat = [&]() {
            Mat m;
            REP(i, K) REP(j, K) m[i][j] = INF;
            return m;
        };
        auto mul = [&](const Mat &a, const Mat &b) {
            Mat c = empty_mat();
            REP(i, N) {
                REP(k, N) {
                    if (a[i][k] >= INF) continue;
                    REP(j, N) {
                        if (b[k][j] >= INF) continue;
                        chmin(c[i][j], a[i][k] + b[k][j]);
                    }
                }
            }

            return c;
        };
        auto advance = [&](const Mat &d, int x) {
            vector<int> len(N);
            vector<char> ok(N);
            REP(q, N) {
                ok[q] = safe(x, q);
                int r = 0;
                while (r < N and safe(x - 1, (q - 1 - r + N) % N)) {
                    ++r;
                }
                len[q] = min(N, r + 1);
            }
            Mat nd = empty_mat();
            REP(s, N) {
                array<ll, 2 * K> val;
                REP(i, 2 * N) {
                    ll z = d[s][i % N];
                    val[i] = z >= INF ? INF : z - i;
                }
                deque<int> dq;
                int ptr = 0;
                REP(q, N) {
                    int Q = N + q;
                    int r = Q - 1;
                    while (ptr <= r) {
                        while (!dq.empty() and val[dq.back()] >= val[ptr]) {
                            dq.pop_back();
                        }
                        dq.push_back(ptr++);
                    }
                    int l = Q - len[q];

                    while (!dq.empty() and dq.front() < l) {
                        dq.pop_front();
                    }
                    if (ok[q] and !dq.empty() and val[dq.front()] < INF) {
                        nd[s][q] = Q + val[dq.front()];
                    }
                }
            }

            return nd;
        };
        Mat P = empty_mat();
        REP(i, N) {
            P[i][i] = 0;
        }
        REP(x, 1, L + 1) {
            P = advance(P, x);
        }
        ll cnt = M / L;
        int rem = M % L;
        array<ll, K> dp;
        dp.fill(INF);
        dp[0] = 0;
        auto apply = [&](array<ll, K> v, const Mat &m) {
            array<ll, K> nv;
            nv.fill(INF);
            REP(i, N) {
                if (v[i] >= INF) continue;
                REP(j, N) {
                    if (m[i][j] >= INF) continue;
                    chmin(nv[j], v[i] + m[i][j]);
                }
            }
            return nv;
        };
        Mat pw = P;
        while (cnt) {
            if (cnt & 1) {
                dp = apply(dp, pw);
            }
            pw = mul(pw, pw);
            cnt >>= 1;
        }
        REP(x, 1, rem + 1) {
            array<ll, K> ndp;
            ndp.fill(INF);
            REP(p, N) {
                if (dp[p] >= INF) continue;
                REP(d, 1, N + 1) {
                    int q = (p + d) % N;
                    if (safe(x, q)) {
                        chmin(ndp[q], dp[p] + d);
                    }
                    if (!safe(x - 1, q)) {
                        break;
                    }
                }
            }
            dp = ndp;
        }
        ll ans = INF;
        REP(p, N) chmin(ans, dp[p]);
        print(ans >= INF ? -1 : ans);
    }
}