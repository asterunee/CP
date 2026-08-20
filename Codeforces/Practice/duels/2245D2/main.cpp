#include "library/template.hpp"

using namespace std;
using namespace suisen;

signed main() {
    int T;
    read(T);
    LOOP(T) {
        int N, M;
        read(N, M);
        int V = 2 * N;
        vector<vector<int>> g(V), rg(V);
        vector<pair<int, int>> bad;
        bad.reserve(M);
        auto add = [&](int u, int v) {
            g[u].push_back(v);
            rg[v].push_back(u);
        };

        LOOP(M) {
            int o, i, j;
            read(o, i, j);
            --i, --j;
            int u = 2 * i;
            int v = 2 * j;
            if (o == 1) {
                add(v ^ 1, u);
                add(u ^ 1, v);
            } else {
                add(u, v ^ 1);
                add(v, u ^ 1);
                bad.emplace_back(u, v);
            }
        }
        vector<char> vis(V);
        vector<int> it(V), ord;
        ord.reserve(V);
        vector<int> st;
        st.reserve(V);
        REP(s, V) {
            if (vis[s]) continue;
            st.clear();
            st.push_back(s);
            vis[s] = true;
            while (!st.empty()) {
                int v = st.back();
                if (it[v] == (int) g[v].size()) {
                    ord.push_back(v);
                    st.pop_back();
                    continue;
                }
                int u = g[v][it[v]++];
                if (!vis[u]) {
                    vis[u] = true;
                    st.push_back(u);
                }
            }
        }
        vector<int> comp(V, -1);
        int k = 0;
        RREP(t, V) {
            int s = ord[t];
            if (comp[s] != -1) continue;
            st.clear();
            st.push_back(s);
            comp[s] = k;
            while (!st.empty()) {
                int v = st.back();
                st.pop_back();
                for (int u : rg[v]) {
                    if (comp[u] == -1) {
                        comp[u] = k;
                        st.push_back(u);
                    }
                }
            }
            ++k;
        }
        bool ok = true;
        for (auto [u, v] : bad) {
            if (comp[u] == comp[v ^ 1] or
                comp[v] == comp[u ^ 1]) {
                ok = false;
                break;
            }
        }
        if (!ok) {
            print("NO");
            continue;
        }
        vector<int> A(N);
        REP(i, N) {
            A[i] = comp[2 * i] - comp[2 * i + 1];
        }
        print("YES");
        print(A);
    }
}