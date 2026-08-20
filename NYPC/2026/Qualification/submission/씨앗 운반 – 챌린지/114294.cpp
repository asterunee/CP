#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

#define rep(i, a, b) for(int i = a; i < b; i++)

int c_val, t_lim, m_val, orig_m;
int oa[15], ob[15];
int a[15], b[15];
int r_val;
int tot_sm;

struct State {
    int ty[12][12];
    int l[12][12];
    int d[12][12][4];
    int tr[12][12], tc[12][12];
} cur, best;

int best_cost = 2e9;
int dx[] = {-1, 0, 1, 0};
int dy[] = {0, 1, 0, -1};

struct Move { int r, c, tr, tc; };

int eval(const State& st, int& out_delay) {
    int q[15][15] = {0};
    int sq_idx[15][15] = {0};
    int saved[15] = {0};
    int t = 1;
    int active = 0, harvested = 0;
    int max_t = min(t_lim, m_val + 50);
    int penalty = 0;

    Move moves[400];

    while (t <= max_t) {
        rep(i, 1, c_val + 1) {
            if (t >= m_val - a[i] + 1 && t <= m_val) {
                q[1][i]++;
                active++;
                harvested++;
            }
        }

        int num_moves = 0;
        bool ov[15][15] = {false};
        bool ov_b[15] = {false};

        rep(r, 1, r_val + 1) {
            rep(c, 1, c_val + 1) {
                if (q[r][c] > 0) {
                    int sent = 0;
                    if (st.ty[r][c] == 0) {
                        int len = st.l[r][c];
                        int limit = q[r][c] < len ? q[r][c] : len;
                        rep(i, 0, limit) {
                            int dir = st.d[r][c][sq_idx[r][c]++];
                            if (sq_idx[r][c] == len) sq_idx[r][c] = 0;
                            int nr = r + dx[dir], nc = c + dy[dir];
                            if (nr >= 1 && nr <= r_val + 1 && nc >= 1 && nc <= c_val) {
                                moves[num_moves++] = {r, c, nr, nc};
                            }
                            sent++;
                        }
                    } else {
                        moves[num_moves++] = {r, c, st.tr[r][c], st.tc[r][c]};
                        sent++;
                    }
                    q[r][c] -= sent;
                    if (q[r][c] > 0) ov[r][c] = true;
                }
            }
        }

        rep(i, 1, c_val + 1) {
            if (q[r_val + 1][i] > 0) ov_b[i] = true;
        }

        rep(i, 0, num_moves) {
            int tr = moves[i].tr, tc = moves[i].tc;
            bool is_ov = (tr == r_val + 1) ? ov_b[tc] : ov[tr][tc];
            if (is_ov) {
                q[moves[i].r][moves[i].c]++;
                penalty += 15;
            } else {
                if (tr == r_val + 1) q[r_val + 1][tc]++;
                else q[tr][tc]++;
            }
        }

        rep(i, 1, c_val + 1) {
            if (q[r_val + 1][i] > 0) {
                q[r_val + 1][i]--;
                saved[i]++;
                active--;
            }
        }

        if (harvested == tot_sm && active == 0) break;
        t++;
    }

    int E = 0, L = 0;
    rep(i, 1, c_val + 1) {
        E += abs(saved[i] - b[i]);
        if (saved[i] < b[i]) L += b[i] - saved[i];
    }

    if (L > 0) out_delay = t_lim;
    else out_delay = max(0, t - m_val);

    return max(E, out_delay) + 2000 * L + penalty;
}

mt19937 rng(1337);

void mutate(State& st) {
    int r = rng() % r_val + 1;
    int c = rng() % c_val + 1;
    if (rng() % 2 == 0) {
        st.ty[r][c] = 0;
        st.l[r][c] = rng() % 4 + 1;
        rep(i, 0, st.l[r][c]) st.d[r][c][i] = rng() % 4;
    } else {
        st.ty[r][c] = 1;
        if (rng() % 2 == 0) {
            st.tr[r][c] = rng() % (r_val + 1) + 1;
            while (st.tr[r][c] == r) st.tr[r][c] = rng() % (r_val + 1) + 1;
            st.tc[r][c] = c;
        } else {
            st.tr[r][c] = r;
            st.tc[r][c] = rng() % c_val + 1;
            while (st.tc[r][c] == c) st.tc[r][c] = rng() % c_val + 1;
        }
    }
}

void solve() {
    cin >> c_val >> t_lim >> orig_m;
    rep(i, 1, c_val + 1) cin >> oa[i];
    rep(i, 1, c_val + 1) cin >> ob[i];

    double scale = 1.0;
    if (t_lim > 3000) {
        scale = orig_m / 150.0;
        if (scale < 1.0) scale = 1.0;
    }

    tot_sm = 0;
    rep(i, 1, c_val + 1) {
        a[i] = round(oa[i] / scale);
        b[i] = round(ob[i] / scale);
        tot_sm += a[i];
    }

    int diff = 0;
    rep(i, 1, c_val + 1) diff += a[i] - b[i];
    while (diff > 0) {
        int bi = 1;
        rep(i, 1, c_val + 1) if (b[i] > b[bi]) bi = i;
        b[bi]++; diff--;
    }
    while (diff < 0) {
        int bi = 1;
        rep(i, 1, c_val + 1) if (a[i] > a[bi]) bi = i;
        a[bi]++; diff++;
    }

    m_val = 0;
    rep(i, 1, c_val + 1) m_val = max(m_val, a[i]);
    if (m_val < 10) m_val = 10;

    r_val = c_val;

    rep(r, 1, r_val + 1) {
        rep(c, 1, c_val + 1) {
            cur.ty[r][c] = 1;
            cur.tr[r][c] = r_val + 1;
            cur.tc[r][c] = c;
        }
    }

    auto start_t = chrono::steady_clock::now();
    int iter = 0, d;
    int cur_cost = eval(cur, d);
    best_cost = cur_cost;
    best = cur;

    double temp = 1000.0;
    uniform_real_distribution<double> dist(0.0, 1.0);

    while (true) {
        if ((iter & 255) == 0) {
            auto now = chrono::steady_clock::now();
            double elap = chrono::duration_cast<chrono::milliseconds>(now - start_t).count();
            if (elap > 1850) break;
            temp = 1000.0 * (1.0 - elap / 1850.0);
            if (temp < 0.1) temp = 0.1;
        }
        iter++;

        State nxt = cur;
        mutate(nxt);
        
        int nd;
        int ncost = eval(nxt, nd);

        if (ncost < cur_cost) {
            cur_cost = ncost;
            cur = nxt;
            if (ncost < best_cost) {
                best_cost = ncost;
                best = nxt;
            }
        } else {
            if (dist(rng) < exp((cur_cost - ncost) / temp)) {
                cur_cost = ncost;
                cur = nxt;
            }
        }
    }

    cout << r_val << "\n";
    rep(r, 1, r_val + 1) {
        rep(c, 1, c_val + 1) {
            if (best.ty[r][c] == 0) {
                string s = "";
                rep(i, 0, best.l[r][c]) {
                    if (best.d[r][c][i] == 0) s += "U";
                    else if (best.d[r][c][i] == 1) s += "R";
                    else if (best.d[r][c][i] == 2) s += "D";
                    else s += "L";
                }
                cout << s << (c == c_val ? "" : " ");
            } else {
                int tr = best.tr[r][c];
                int tc = best.tc[r][c];
                if (tr == r) {
                    if (tc > c) cout << (tc - c) << "R";
                    else cout << (c - tc) << "L";
                } else {
                    if (tr > r) cout << (tr - r) << "D";
                    else cout << (r - tr) << "U";
                }
                cout << (c == c_val ? "" : " ");
            }
        }
        cout << "\n";
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    solve();
    return 0;
}