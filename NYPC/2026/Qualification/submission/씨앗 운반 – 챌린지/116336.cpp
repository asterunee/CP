#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

inline double get_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}

string jump(int dist, string dir) {
    if (dist == 0) return "X";
    if (dist == 1) return dir;
    return to_string(dist) + dir;
}

uint32_t xor_seed = 13371337;
inline uint32_t fast_rand() {
    xor_seed ^= xor_seed << 13;
    xor_seed ^= xor_seed >> 17;
    xor_seed ^= xor_seed << 5;
    return xor_seed;
}

const int LOG_TABLE_SIZE = 65536;
double log_table[LOG_TABLE_SIZE];
bool log_table_initialized = false;

void init_log_table() {
    if (log_table_initialized) return;
    mt19937 gen(1337);
    uniform_real_distribution<double> dist(0.00001, 1.0);
    for (int i = 0; i < LOG_TABLE_SIZE; i++) {
        log_table[i] = -log(dist(gen)); 
    }
    log_table_initialized = true;
}

inline ll evaluate(const int* T_arr, const int* order, int n, const ll* a, const ll* a_c, const ll* a_f, const ll* b, int m) {
    ll err = 0;
    int splits = 0;
    ll rec[15] = {0}; 
    
    int used[30] = {0}; 
    int y_curr = 2;
    int par[15];
    for (int i = 0; i < n; i++) par[i] = -1;
    bool fix = false;

    for (int idx = 0; idx < n; ++idx) {
        int i = order[idx];
        int b1 = T_arr[i << 1];
        int b2 = T_arr[(i << 1) | 1];
        int p_s = (m - a[i] + 1) % 2;
        
        if (b1 == b2) {
            rec[b1] += a[i];
            
            int mn = (i < b1) ? i : b1;
            int mx = (i > b1) ? i : b1;
            int mask = ((1 << (mx - mn + 1)) - 1) << mn;
            int y = 1;
            while (used[y] & mask) y++;
            used[y] |= mask;
            if (y + 2 > y_curr) y_curr = y + 2;
            
            int p = (i == b1) ? p_s : (p_s ^ 1);
            if (par[b1] != -1 && par[b1] != p) fix = true;
            par[b1] = p;
        } else {
            splits++;
            rec[b1] += a_c[i];
            rec[b2] += a_f[i];
            
            int col2 = (i == n - 1) ? i - 1 : i + 1;
            int L1 = (i < b1) ? i : b1;
            int R1 = (i > b1) ? i : b1;
            int mask1 = ((1 << (R1 - L1 + 1)) - 1) << L1;
            
            int mn2 = i;
            if (col2 < mn2) mn2 = col2;
            if (b2 < mn2) mn2 = b2;
            int mx2 = i;
            if (col2 > mx2) mx2 = col2;
            if (b2 > mx2) mx2 = b2;
            int mask2 = ((1 << (mx2 - mn2 + 1)) - 1) << mn2;
            
            int y = 1;
            while (true) {
                if (used[y] & mask2) { y++; continue; }
                if (used[y + 1] & mask1) { y++; continue; }
                break;
            }
            used[y] |= mask2;
            used[y + 1] |= mask1;
            if (y + 3 > y_curr) y_curr = y + 3;
            
            int p1 = (i == b1) ? (p_s ^ 1) : p_s;
            if (par[b1] != -1 && par[b1] != p1) fix = true;
            par[b1] = p1;
            
            int p2 = (col2 == b2) ? p_s : (p_s ^ 1);
            if (par[b2] != -1 && par[b2] != p2) fix = true;
            par[b2] = p2;
        }
    }
    
    for (int i = 0; i < n; ++i) {
        ll diff = rec[i] - b[i];
        if (diff > 0) err += diff;
        else err -= diff;
    }
    
    int R_val = fix ? y_curr : (y_curr - 1); 
    ll delay = fix ? 3 : 2;
    ll max_ed = (err > delay) ? err : delay;
    ll true_cost = 2 * R_val - n + max_ed;
    
    return true_cost * 10000000LL + err * 10000LL + splits * 100LL;
}

void solve() {
    int n, t_total, m; cin >> n >> t_total >> m;

    vector<ll> a(n), b(n);
    ll a_arr[15], a_c[15], a_f[15], b_arr[15];
    
    for (int i = 0; i < n; i++) {
        cin >> a[i];
        a_arr[i] = a[i];
        a_f[i] = a[i] >> 1;
        a_c[i] = (a[i] + 1) >> 1;
    }
    for (int i = 0; i < n; i++) {
        cin >> b[i];
        b_arr[i] = b[i];
    }

    init_log_table();

    int current_T[24], best_T[24];
    int ord[15], bord[15];

    vector<pair<ll, int>> pa(n), pb(n);
    for (int i = 0; i < n; ++i) { 
        pa[i] = {a[i], i}; 
        pb[i] = {b[i], i}; 
    }
    sort(pa.begin(), pa.end());
    sort(pb.begin(), pb.end());
    
    for (int i = 0; i < n; ++i) {
        current_T[2 * pa[i].second] = pb[i].second;
        current_T[2 * pa[i].second + 1] = pb[i].second;
        ord[i] = i;
    }

    for (int i = n - 1; i > 0; --i) {
        int j = fast_rand() % (i + 1);
        swap(ord[i], ord[j]);
    }

    ll cur = evaluate(current_T, ord, n, a_arr, a_c, a_f, b_arr, m);
    ll best = cur;
    
    for (int i = 0; i < 2 * n; i++) best_T[i] = current_T[i];
    for (int i = 0; i < n; i++) bord[i] = ord[i];

    double start_time = get_time();
    double max_time = 1.90; 
    double start_temp = 3e7;
    double end_temp = 10.0;
    double temp = start_temp;
    int iter = 0;

    while (true) {
        if ((iter & 16383) == 0) {
            double current_time = get_time();
            if (current_time - start_time > max_time) break;
            double progress = (current_time - start_time) / max_time;
            temp = start_temp * pow(end_temp / start_temp, progress);
        }
        iter++;

        int type = fast_rand() % 100;
        bool accept = false;
        
        if (type < 40) {
            int u = fast_rand() % (2 * n);
            int v = fast_rand() % (2 * n);
            if (u != v && current_T[u] != current_T[v]) {
                swap(current_T[u], current_T[v]);
                ll new_score = evaluate(current_T, ord, n, a_arr, a_c, a_f, b_arr, m);
                if (new_score <= cur || (new_score - cur) < temp * log_table[fast_rand() & 65535]) {
                    accept = true;
                    cur = new_score;
                } else {
                    swap(current_T[u], current_T[v]);
                }
            }
        } else if (type < 70) {
            int u = fast_rand() % (2 * n);
            int v = fast_rand() % (2 * n);
            int w = fast_rand() % (2 * n);
            if (u != v && v != w && u != w) {
                int tmp = current_T[u];
                current_T[u] = current_T[v];
                current_T[v] = current_T[w];
                current_T[w] = tmp;
                ll new_score = evaluate(current_T, ord, n, a_arr, a_c, a_f, b_arr, m);
                if (new_score <= cur || (new_score - cur) < temp * log_table[fast_rand() & 65535]) {
                    accept = true;
                    cur = new_score;
                } else {
                    tmp = current_T[w];
                    current_T[w] = current_T[v];
                    current_T[v] = current_T[u];
                    current_T[u] = tmp;
                }
            }
        } else if (type < 85) {
            int u = fast_rand() % (2 * n);
            int v = fast_rand() % (2 * n);
            int w = fast_rand() % (2 * n);
            int x = fast_rand() % (2 * n);
            if (u != v && u != w && u != x && v != w && v != x && w != x) {
                int tmp = current_T[u];
                current_T[u] = current_T[v];
                current_T[v] = current_T[w];
                current_T[w] = current_T[x];
                current_T[x] = tmp;
                ll new_score = evaluate(current_T, ord, n, a_arr, a_c, a_f, b_arr, m);
                if (new_score <= cur || (new_score - cur) < temp * log_table[fast_rand() & 65535]) {
                    accept = true;
                    cur = new_score;
                } else {
                    tmp = current_T[x];
                    current_T[x] = current_T[w];
                    current_T[w] = current_T[v];
                    current_T[v] = current_T[u];
                    current_T[u] = tmp;
                }
            }
        } else {
            if (fast_rand() % 2 == 0) {
                int u = fast_rand() % n;
                int v = fast_rand() % n;
                if (u != v) {
                    swap(ord[u], ord[v]);
                    ll new_score = evaluate(current_T, ord, n, a_arr, a_c, a_f, b_arr, m);
                    if (new_score <= cur || (new_score - cur) < temp * log_table[fast_rand() & 65535]) {
                        accept = true;
                        cur = new_score;
                    } else {
                        swap(ord[u], ord[v]);
                    }
                }
            } else {
                int u = fast_rand() % n;
                int v = fast_rand() % n;
                int w = fast_rand() % n;
                if (u != v && v != w && u != w) {
                    int tmp = ord[u];
                    ord[u] = ord[v];
                    ord[v] = ord[w];
                    ord[w] = tmp;
                    ll new_score = evaluate(current_T, ord, n, a_arr, a_c, a_f, b_arr, m);
                    if (new_score <= cur || (new_score - cur) < temp * log_table[fast_rand() & 65535]) {
                        accept = true;
                        cur = new_score;
                    } else {
                        tmp = ord[w];
                        ord[w] = ord[v];
                        ord[v] = ord[u];
                        ord[u] = tmp;
                    }
                }
            }
        }

        if (accept && cur < best) {
            best = cur;
            for (int k = 0; k < 2 * n; k++) best_T[k] = current_T[k];
            for (int k = 0; k < n; k++) bord[k] = ord[k];
        }
    }

    vector<int> final_P(2 * n);
    for (int i = 0; i < n; i++) {
        final_P[2 * i] = best_T[2 * i] * 2;
        final_P[2 * i + 1] = best_T[2 * i + 1] * 2;
    }

    vector<vector<int>> route(n);
    int y_curr = 2; 
    vector<vector<bool>> used(100, vector<bool>(n, false));
    
    auto try_place = [&](int r, int c1, int c2) {
        int mn = min(c1, c2), mx = max(c1, c2);
        for (int c = mn; c <= mx; ++c) if (used[r][c]) return false;
        return true;
    };
    
    auto place = [&](int r, int c1, int c2) {
        int mn = min(c1, c2), mx = max(c1, c2);
        for (int c = mn; c <= mx; ++c) used[r][c] = true;
    };

    for (int idx = 0; idx < n; idx++) {
        int i = bord[idx];
        int b1 = final_P[2 * i] / 2;
        int b2 = final_P[2 * i + 1] / 2;
        if (b1 == b2) {
            int y = 2;
            while (!try_place(y - 1, i, b1)) y++;
            place(y - 1, i, b1);
            route[i] = {y};
            y_curr = max(y_curr, y + 1);
        } else {
            int col2 = (i == n - 1) ? i - 1 : i + 1;
            int y = 2;
            while (true) {
                if (!try_place(y - 1, min({i, col2, b2}), max({i, col2, b2}))) { y++; continue; }
                if (!try_place(y, min(i, b1), max(i, b1))) { y++; continue; }
                break;
            }
            place(y - 1, min({i, col2, b2}), max({i, col2, b2}));
            place(y, min(i, b1), max(i, b1));
            route[i] = {y, y + 1};
            y_curr = max(y_curr, y + 2);
        }
    }
    bool fix = false;
    vector<int> par(n, -1);
    
    auto check_parity = [&](int b_idx, int arr_p) {
        if (par[b_idx] == arr_p) {
            fix = true;
            par[b_idx] = 1 - arr_p;
        } else {
            par[b_idx] = arr_p;
        }
    };

    for (int idx = 0; idx < n; idx++) {
        int i = bord[idx];
        int p_s = (m - a[i] + 1) % 2;
        int b1 = final_P[2 * i] / 2;
        int b2 = final_P[2 * i + 1] / 2;
        
        if (b1 == b2) {
            if (i == b1) check_parity(b1, (p_s + 2) % 2);
            else check_parity(b1, (p_s + 3) % 2);
        } else {
            if (i == b1) check_parity(b1, (p_s + 1) % 2);
            else check_parity(b1, (p_s + 2) % 2);
            
            int col2 = (i == n - 1) ? i - 1 : i + 1;
            if (col2 == b2) check_parity(b2, (p_s + 2) % 2);
            else check_parity(b2, (p_s + 3) % 2);
        }
    }

    int y_fix = fix ? y_curr : -1;
    int y_burrow = fix ? y_curr + 1 : y_curr;
    int rows = y_burrow - 1; 

    vector<vector<string>> g(rows, vector<string>(n, "X"));
    vector<int> burrow_parity(n, -1);

    auto add_delay = [&](int b_idx, int arr_parity, int base_row, int col) {
        if (burrow_parity[b_idx] == arr_parity) {
            g[base_row - 1][col] = jump(y_fix - base_row, "D");
            g[y_fix - 1][b_idx] = "D";
            burrow_parity[b_idx] = 1 - arr_parity;
        } else {
            g[base_row - 1][col] = jump(y_burrow - base_row, "D");
            burrow_parity[b_idx] = arr_parity;
        }
    };
    for (int i = 0; i < n; i++) {
        int t_start = m - a[i] + 1;
        int p_s = t_start % 2;
        int b1 = final_P[2 * i] / 2;
        int b2 = final_P[2 * i + 1] / 2;

        if (b1 == b2) { 
            int y = route[i][0];
            g[0][i] = jump(y - 1, "D");

            if (i == b1) {
                add_delay(b1, (p_s + 2) % 2, y, i);
            } else {
                g[y - 1][i] = jump(abs(b1 - i), (b1 > i ? "R" : "L"));
                add_delay(b1, (p_s + 3) % 2, y, b1);
            }
        } else { 
            int y = route[i][0];
            g[0][i] = jump(y - 1, "D");
            g[y - 1][i] = (i == n - 1) ? "DL" : "DR"; 
            
            if (i == b1) {
                add_delay(b1, (p_s + 1) % 2, y + 1, i);
            } else {
                g[y][i] = jump(abs(b1 - i), b1 > i ? "R" : "L");
                add_delay(b1, (p_s + 2) % 2, y + 1, b1);
            }

            int col2 = (i == n - 1) ? i - 1 : i + 1;
            if (col2 == b2) {
                add_delay(b2, (p_s + 2) % 2, y, col2);
            } else {
                g[y - 1][col2] = jump(abs(b2 - col2), b2 > col2 ? "R" : "L");
                add_delay(b2, (p_s + 3) % 2, y, b2);
            }
        }
    }
    cout << rows << "\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < n; j++) {
            cout << g[i][j] << (j == n - 1 ? "" : " ");
        }
        cout << "\n";
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T = 1;
    while (T--) solve();
    return 0;
}