#include "library/template.hpp"

using namespace std;
using namespace suisen;
using ll = long long;
constexpr ll MOD = 998244353;

ll pw(ll a, ll b) {
    ll r = 1;
    while (b) {
        if (b & 1) r = r * a % MOD;
        a = a * a % MOD;
        b >>= 1;
    }
    return r;
}

using Mat = vector<vector<ll>>;

Mat mul(const Mat &a, const Mat &b) {
    int n = a.size();
    Mat c(n, vector<ll>(n));
    REP(i, n) REP(k, n) {
        if (!a[i][k]) continue;
        REP(j, n) {
            c[i][j] += a[i][k] * b[k][j] % MOD;
            if (c[i][j] >= MOD) c[i][j] -= MOD;
        }
    }
    return c;
}

signed main() {
    int N, M;
    read(N, M);
    vector<pair<int, int>> E(M);
    vector<int> deg(N);
    REP(i, M) {
        int u, v;
        read(u, v);
        --u, --v;

        E[i] = {u, v};
        ++deg[u];
        ++deg[v];
    }
    ll S, T;
    int A, B;
    read(S, T, A, B);
    --A, --B;
    Mat P(N, vector<ll>(N));
    for (auto [u, v] : E) {
        P[u][v] = pw(deg[u], MOD - 2);
        P[v][u] = pw(deg[v], MOD - 2);
    }
    vector<Mat> p(61);
    p[0] = P;
    REP(k, 1, 61) {
        p[k] = mul(p[k - 1], p[k - 1]);
    }
    auto walk = [&](int s, ll t) {
        vector<ll> v(N);
        v[s] = 1;
        REP(k, 61) {
            if (!(t >> k & 1)) continue;
            vector<ll> nv(N);
            REP(i, N) {
                if (!v[i]) continue;
                REP(j, N) {
                    nv[j] += v[i] * p[k][i][j] % MOD;
                    if (nv[j] >= MOD) nv[j] -= MOD;
                }
            }
            v.swap(nv);
        }
        return v;
    };
    auto x = walk(0, T - 1);
    auto y = walk(B, S - T);
    auto z = walk(0, S - 1);
    ll num = x[B] * y[A] % MOD;
    ll ans = num * pw(z[A], MOD - 2) % MOD;
    print(ans);
}