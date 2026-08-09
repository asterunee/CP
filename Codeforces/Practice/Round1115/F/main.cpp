#include "library/template.hpp"
#include "library/graph/csr_graph.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"

using namespace std;
using namespace suisen;

using ll = long long;

int kth(FenwickTree<ll>& ft, ll x) {
    auto& a = ft.get_internal_container();

    int n = ft.size();
    int p = 0;
    ll s = 0;

    int k = 1;
    while ((k << 1) <= n) {
        k <<= 1;
    }

    for (; k; k >>= 1) {
        if (p + k <= n and s + a[p + k - 1] < x) {
            s += a[p + k - 1];
            p += k;
        }
    }

    return p;
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    read(T);
    while (T--) {
        int N;
        read(N);
        vector<int> C(N), K(N);
        read(C);
        read(K);
        for (int& c : C) {
            --c;
        }
        vector<pair<int, int>> E(N - 1);
        for (auto& [u, v] : E) {
            read(u, v);
            --u, --v;
        }
        auto G = Graph<>::create_undirected_graph(N, E);
        int LOG = 1;
        while ((1 << LOG) <= N) {
            ++LOG;
        }
        vector<vector<int>> up(LOG, vector<int>(N));
        vector<int> dep(N), tin(N), siz(N, 1), ord;
        ord.reserve(N);
        vector<int> st{0};
        up[0][0] = 0;
        while (st.size()) {
            int u = st.back();
            st.pop_back();
            tin[u] = ord.size();
            ord.push_back(u);
            for (int v : G[u]) {
                if (v == up[0][u]) {
                    continue;
                }
                up[0][v] = u;
                dep[v] = dep[u] + 1;
                st.push_back(v);
            }
        }
        RREP(i, 1, N) {
            int u = ord[i];
            siz[up[0][u]] += siz[u];
        }
        REP(j, 1, LOG) {
            REP(v, N) {
                up[j][v] = up[j - 1][up[j - 1][v]];
            }
        }
        auto anc = [&](int u, int v) {
            return tin[u] <= tin[v] and tin[v] < tin[u] + siz[u];
        };
        auto lca = [&](int u, int v) {
            if (anc(u, v)) {
                return u;
            }
            if (anc(v, u)) {
                return v;
            }
            RREP(j, LOG) {
                if (!anc(up[j][u], v)) {
                    u = up[j][u];
                }
            }
            return up[0][u];
        };
        vector<vector<int>> col(N);
        REP(v, N) {
            col[C[v]].push_back(v);
        }
        vector<ll> ans(N, -1);
        REP(c, N) {
            int m = col[c].size();
            if (m == 0) {
                continue;
            }
            int q = K[c] - 1;
            vector<int> vs = col[c];
            sort(ALL(vs), [&](int u, int v) {
                return tin[u] < tin[v];
            });
            int z = vs.size();
            REP(i, 1, z) {
                vs.push_back(lca(vs[i - 1], vs[i]));
            }
            sort(ALL(vs), [&](int u, int v) {
                return tin[u] < tin[v];
            });
            vs.erase(unique(ALL(vs)), vs.end());
            int S = vs.size();
            vector<int> par(S, -1), stk;
            stk.reserve(S);
            REP(i, S) {
                while (stk.size() and !anc(vs[stk.back()], vs[i])) {
                    stk.pop_back();
                }
                if (stk.size()) {
                    par[i] = stk.back();
                }
                stk.push_back(i);
            }
            vector<int> cnt(S);
            REP(i, S) {
                cnt[i] = C[vs[i]] == c;
            }
            RREP(i, 1, S) {
                cnt[par[i]] += cnt[i];
            }
            vector<vector<int>> ch(S);
            vector<int> len(S);
            FenwickTree<ll> fc(m + 1);
            FenwickTree<ll> fs(m + 1);
            ll ec = 0;
            REP(i, 1, S) {
                int p = par[i];
                ch[p].push_back(i);
                len[i] = dep[vs[i]] - dep[vs[p]];
                int w = cnt[i];
                fc.add(w, len[i]);
                fs.add(w, 1LL * len[i] * w);
                ec += len[i];
            }
            auto cost = [&]() -> ll {
                ll need = ec - q;
                if (need == 0) {
                    return 0;
                }
                int w = kth(fc, need);
                ll cc = fc.sum(0, w);
                ll ss = fs.sum(0, w);
                return ss + (need - cc) * w;
            };
            auto add = [&](int w, int d) {
                fc.add(w, d);
                fs.add(w, 1LL * w * d);
            };
            ll best = cost();
            vector<int> it(S);
            vector<int> dfs{0};
            while (dfs.size()) {
                int u = dfs.back();
                if (it[u] == int(ch[u].size())) {
                    dfs.pop_back();
                    if (u != 0) {
                        int w = cnt[u];
                        int d = len[u];

                        add(m - w, -d);
                        add(w, d);
                    }
                    continue;
                }
                int v = ch[u][it[u]++];
                int w = cnt[v];
                int d = len[v];
                add(w, -d);
                add(m - w, d);
                chmin(best, cost());
                dfs.push_back(v);
            }
            ans[c] = best;
        }
        print(ans);
    }
}