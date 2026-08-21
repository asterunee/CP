#include "library/template.hpp"

using namespace std;
using namespace suisen;

signed main() {
    int N, D;
    read(N, D);
    vector<int> A(N), B(N), C(N);
    REP(i, N) {
        read(A[i], B[i], C[i]);
    }
    int S = 1 << N;
    vector<int> sa(S);
    REP(mask, 1, S) {
        int b = mask & -mask;
        int i = __builtin_ctz(b);
        sa[mask] = sa[mask ^ b] + A[i];
    }
    vector<vector<pair<int, int>>> dp(S);
    dp[0].push_back({0, 0});
    REP(mask, S) {
        auto &v = dp[mask];
        if (v.empty()) continue;
        sort(ALL(v));
        vector<pair<int, int>> nv;
        int best = 1e9;
        for (auto [x, y] : v) {
            if (y > D) continue;
            if (y < best) {
                nv.push_back({x, y});
                best = y;
            }
        }
        v.swap(nv);
        REP(i, N) {
            if (mask >> i & 1) continue;
            int nmask = mask | 1 << i;
            int t1 = sa[nmask];
            for (auto [t2, t3] : v) {
                int nt2 = max(t1, t2) + B[i];
                int nt3 = max(nt2, t3) + C[i];
                if (nt3 <= D) {
                    dp[nmask].push_back({nt2, nt3});
                }
            }
        }
    }
    print(dp[S - 1].empty() ? "No" : "Yes");
}