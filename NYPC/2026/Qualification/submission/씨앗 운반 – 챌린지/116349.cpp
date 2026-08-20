#include <bits/stdc++.h>
using namespace std;
using ll = long long;

const ll INF = 4e18;

double get_time() {
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
        log_table[i] = log(dist(gen));
    }
    log_table_initialized = true;
}

void solve() {
    int n, t_total, m; cin >> n >> t_total >> m;

    vector<ll> a(n), b(n);
    for (int i = 0; i < n; i++) cin >> a[i];
    for (int i = 0; i < n; i++) cin >> b[i];

    init_log_table();

    int P[24], best_P[24];
    ll received[12] = {0};
    ll b_arr[12];
    ll a_f[12], a_c[12]; 
    
    for (int i = 0; i < n; i++) {
        b_arr[i] = b[i];
        a_f[i] = a[i] >> 1;
        a_c[i] = (a[i] + 1) >> 1;
    }

    vector<pair<ll, int>> sorted_a(n), sorted_b(n);
    for(int i=0; i<n; ++i) { sorted_a[i] = {a[i], i}; sorted_b[i] = {b[i], i}; }
    sort(sorted_a.begin(), sorted_a.end());
    sort(sorted_b.begin(), sorted_b.end());
    
    for(int i=0; i<n; ++i) {
        P[2 * sorted_a[i].second] = 2 * sorted_b[i].second;
        P[2 * sorted_a[i].second + 1] = 2 * sorted_b[i].second + 1;
    }

    int split_count = 0;
    for (int i = 0; i < n; i++) {
        int b1 = P[2 * i] >> 1;
        int b2 = P[2 * i + 1] >> 1;
        if (b1 == b2) {
            received[b1] += a[i];
        } else {
            received[b1] += a_c[i];
            received[b2] += a_f[i];
            split_count++;
        }
    }
    
    ll current_err = 0;
    for (int i = 0; i < n; i++) current_err += abs(received[i] - b_arr[i]);
    ll current_score = current_err * 1000LL + split_count;

    ll best_score = current_score;
    for (int i = 0; i < 2 * n; i++) best_P[i] = P[i];

    double start_time = get_time();
    
    double start_temp = 100000.0;
    double end_temp = 0.1;
    double max_time = 1.85;

    int iter = 0;
    double temp = start_temp;
    int max_u = 2 * n;

    while (true) {
        if ((iter & 16383) == 0) {
            double current_time = get_time();
            if (current_time - start_time > max_time) break;
            double progress = (current_time - start_time) / max_time;
            temp = start_temp * pow(end_temp / start_temp, progress);
        }
        iter++;

        if (best_score == 0) break; 

        int u = (fast_rand() * (uint64_t)max_u) >> 32;
        int v = (fast_rand() * (uint64_t)max_u) >> 32;
        if (u == v) continue;
        
        int i1 = u >> 1, i2 = v >> 1;

        if (i1 == i2) { 
            if ((a[i1] & 1) == 0) continue; 
            int old_b1 = P[2 * i1] >> 1;
            int old_b2 = P[2 * i1 + 1] >> 1;
            if (old_b1 == old_b2) continue;
            
            ll new_err = current_err;
            new_err -= abs(received[old_b1] - b_arr[old_b1]);
            new_err -= abs(received[old_b2] - b_arr[old_b2]);

            received[old_b1]--;
            received[old_b2]++;

            new_err += abs(received[old_b1] - b_arr[old_b1]);
            new_err += abs(received[old_b2] - b_arr[old_b2]);

            ll new_score = new_err * 1000LL + split_count;

            bool accept = (new_score < current_score);
            if (!accept) {
                if ((current_score - new_score) > temp * log_table[fast_rand() & 65535]) accept = true;
            }

            if (accept) {
                current_err = new_err;
                current_score = new_score;
                swap(P[u], P[v]);
                if (current_score < best_score) {
                    best_score = current_score;
                    for (int k = 0; k < 2 * n; k++) best_P[k] = P[k];
                }
            } else { 
                received[old_b1]++;
                received[old_b2]--;
            }
        } else {
            int old_b1_1 = P[2 * i1] >> 1, old_b2_1 = P[2 * i1 + 1] >> 1;
            int old_b1_2 = P[2 * i2] >> 1, old_b2_2 = P[2 * i2 + 1] >> 1;

            uint32_t mask = (1 << old_b1_1) | (1 << old_b2_1) | (1 << old_b1_2) | (1 << old_b2_2);
            ll new_err = current_err;

            uint32_t t_mask = mask;
            while (t_mask) {
                int idx = __builtin_ctz(t_mask);
                new_err -= abs(received[idx] - b_arr[idx]);
                t_mask &= t_mask - 1;
            }

            int old_split = split_count;
            if (old_b1_1 == old_b2_1) received[old_b1_1] -= a[i1];
            else { received[old_b1_1] -= a_c[i1]; received[old_b2_1] -= a_f[i1]; split_count--; }

            if (old_b1_2 == old_b2_2) received[old_b1_2] -= a[i2];
            else { received[old_b1_2] -= a_c[i2]; received[old_b2_2] -= a_f[i2]; split_count--; }

            swap(P[u], P[v]);
            int new_b1_1 = P[2 * i1] >> 1, new_b2_1 = P[2 * i1 + 1] >> 1;
            int new_b1_2 = P[2 * i2] >> 1, new_b2_2 = P[2 * i2 + 1] >> 1;

            if (new_b1_1 == new_b2_1) received[new_b1_1] += a[i1];
            else { received[new_b1_1] += a_c[i1]; received[new_b2_1] += a_f[i1]; split_count++; }

            if (new_b1_2 == new_b2_2) received[new_b1_2] += a[i2];
            else { received[new_b1_2] += a_c[i2]; received[new_b2_2] += a_f[i2]; split_count++; }

            t_mask = mask;
            while (t_mask) {
                int idx = __builtin_ctz(t_mask);
                new_err += abs(received[idx] - b_arr[idx]);
                t_mask &= t_mask - 1;
            }

            ll new_score = new_err * 1000LL + split_count;

            bool accept = (new_score < current_score);
            if (!accept) {
                if ((current_score - new_score) > temp * log_table[fast_rand() & 65535]) accept = true;
            }

            if (accept) {
                current_err = new_err;
                current_score = new_score;
                if (current_score < best_score) {
                    best_score = current_score;
                    for (int k = 0; k < 2 * n; k++) best_P[k] = P[k];
                }
            } else { 
                if (new_b1_1 == new_b2_1) received[new_b1_1] -= a[i1];
                else { received[new_b1_1] -= a_c[i1]; received[new_b2_1] -= a_f[i1]; }

                if (new_b1_2 == new_b2_2) received[new_b1_2] -= a[i2];
                else { received[new_b1_2] -= a_c[i2]; received[new_b2_2] -= a_f[i2]; }

                swap(P[u], P[v]);
                split_count = old_split;

                if (old_b1_1 == old_b2_1) received[old_b1_1] += a[i1];
                else { received[old_b1_1] += a_c[i1]; received[old_b2_1] += a_f[i1]; }

                if (old_b1_2 == old_b2_2) received[old_b1_2] += a[i2];
                else { received[old_b1_2] += a_c[i2]; received[old_b2_2] += a_f[i2]; }
            }
        }
    }

    vector<int> final_P(2 * n);
    for (int i = 0; i < 2 * n; i++) final_P[i] = best_P[i];

    vector<vector<int>> routing(n);
    int y_curr = 2; 
    vector<vector<bool>> row_used(100, vector<bool>(n, false));
    
    auto try_place = [&](int r, int c1, int c2) {
        int mn = min(c1, c2), mx = max(c1, c2);
        for(int c=mn; c<=mx; ++c) if (row_used[r][c]) return false;
        return true;
    };
    auto place = [&](int r, int c1, int c2) {
        int mn = min(c1, c2), mx = max(c1, c2);
        for(int c=mn; c<=mx; ++c) row_used[r][c] = true;
    };

    for (int i = 0; i < n; i++) {
        int b1 = final_P[2 * i] / 2;
        int b2 = final_P[2 * i + 1] / 2;
        if (b1 == b2) {
            int y = 2;
            while (!try_place(y - 1, i, b1)) y++;
            place(y - 1, i, b1);
            routing[i] = {y};
            y_curr = max(y_curr, y + 1);
        } else {
            int col2 = (i == n - 1) ? i - 1 : i + 1;
            int y = 2;
            while (true) {
                if (!try_place(y - 1, min(i, min(col2, b2)), max(i, max(col2, b2)))) { y++; continue; }
                if (!try_place(y, i, b1)) { y++; continue; }
                break;
            }
            place(y - 1, min(i, min(col2, b2)), max(i, max(col2, b2)));
            place(y, i, b1);
            routing[i] = {y, y + 1};
            y_curr = max(y_curr, y + 2);
        }
    }

    bool need_fix = false;
    vector<int> temp_parity(n, -1);
    auto check_parity = [&](int b, int arr_p) {
        if (temp_parity[b] == arr_p) {
            need_fix = true;
            temp_parity[b] = 1 - arr_p;
        } else {
            temp_parity[b] = arr_p;
        }
    };

    for (int i = 0; i < n; i++) {
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

    int y_fix = need_fix ? y_curr : -1;
    int y_burrow = need_fix ? y_curr + 1 : y_curr;
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
            int y = routing[i][0];
            g[0][i] = jump(y - 1, "D");

            if (i == b1) {
                add_delay(b1, (p_s + 2) % 2, y, i);
            } else {
                g[y - 1][i] = jump(abs(b1 - i), (b1 > i ? "R" : "L"));
                add_delay(b1, (p_s + 3) % 2, y, b1);
            }
        } else { 
            int y = routing[i][0];
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