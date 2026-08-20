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
        log_table[i] = -log(dist(gen)); 
    }
    log_table_initialized = true;
}

inline ll evaluate(const int* T_arr, int n, const ll* a, const ll* a_c, const ll* a_f, const ll* b, int m) {
    ll err = 0;
    int splits = 0;
    int density[15] = {0};
    ll rec[15] = {0};
    int temp_parity[15];
    for (int i = 0; i < n; i++) temp_parity[i] = -1;
    bool need_fix = false;

    for (int i = 0; i < n; ++i) {
        int b1 = T_arr[2 * i];
        int b2 = T_arr[2 * i + 1];
        int p_s = (m - a[i] + 1) % 2;
        
        if (b1 == b2) {
            rec[b1] += a[i];
            int L = b1 < i ? b1 : i;
            int R = b1 > i ? b1 : i;
            for (int k = L; k <= R; ++k) density[k]++;
            
            int p = (i == b1) ? (p_s + 2) % 2 : (p_s + 3) % 2;
            if (temp_parity[b1] != -1 && temp_parity[b1] != p) need_fix = true;
            temp_parity[b1] = p;
        } else {
            splits++;
            rec[b1] += a_c[i];
            rec[b2] += a_f[i];
            
            int L1 = b1 < i ? b1 : i;
            int R1 = b1 > i ? b1 : i;
            for (int k = L1; k <= R1; ++k) density[k]++;
            
            int col2 = (i == n - 1) ? i - 1 : i + 1;
            int mn2 = i < col2 ? i : col2;
            mn2 = mn2 < b2 ? mn2 : b2;
            int mx2 = i > col2 ? i : col2;
            mx2 = mx2 > b2 ? mx2 : b2;
            
            for (int k = mn2; k <= mx2; ++k) density[k]++;
            
            int p1 = (i == b1) ? (p_s + 1) % 2 : (p_s + 2) % 2;
            if (temp_parity[b1] != -1 && temp_parity[b1] != p1) need_fix = true;
            temp_parity[b1] = p1;
            
            int p2 = (col2 == b2) ? (p_s + 2) % 2 : (p_s + 3) % 2;
            if (temp_parity[b2] != -1 && temp_parity[b2] != p2) need_fix = true;
            temp_parity[b2] = p2;
        }
    }
    
    for (int i = 0; i < n; ++i) {
        ll diff = rec[i] - b[i];
        err += (diff > 0 ? diff : -diff);
    }
    
    int max_d = 0;
    for (int i = 0; i < n; ++i) if (density[i] > max_d) max_d = density[i];
    
    return err * 100000LL + max_d * 200000LL + need_fix * 200000LL + splits * 1000LL;
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
    vector<pair<ll, int>> sorted_a(n), sorted_b(n);
    for (int i = 0; i < n; ++i) { 
        sorted_a[i] = {a[i], i}; 
        sorted_b[i] = {b[i], i}; 
    }
    sort(sorted_a.begin(), sorted_a.end());
    sort(sorted_b.begin(), sorted_b.end());
    
    for (int i = 0; i < n; ++i) {
        current_T[2 * sorted_a[i].second] = sorted_b[i].second;
        current_T[2 * sorted_a[i].second + 1] = sorted_b[i].second;
    }

    ll current_score = evaluate(current_T, n, a_arr, a_c, a_f, b_arr, m);
    ll best_score = current_score;
    for (int i = 0; i < 2 * n; i++) best_T[i] = current_T[i];

    double start_time = get_time();
    double max_time = 1.85;
    double start_temp = 1e8;
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

        int type = fast_rand() % 4;
        bool accept = false;
        ll new_score = 0;
        
        int u = fast_rand() % (2 * n);
        int v = fast_rand() % (2 * n);
        
        if (type < 3) { 
            if (u == v || current_T[u] == current_T[v]) continue;
            swap(current_T[u], current_T[v]);
            
            new_score = evaluate(current_T, n, a_arr, a_c, a_f, b_arr, m);
            if (new_score <= current_score) accept = true;
            else if ((new_score - current_score) < temp * log_table[fast_rand() & 65535]) accept = true;
            
            if (!accept) swap(current_T[u], current_T[v]);
            
        } else { 
            int w = fast_rand() % (2 * n);
            if (u == v || v == w || u == w) continue;
            
            int tmp = current_T[u];
            current_T[u] = current_T[v];
            current_T[v] = current_T[w];
            current_T[w] = tmp;
            
            new_score = evaluate(current_T, n, a_arr, a_c, a_f, b_arr, m);
            if (new_score <= current_score) accept = true;
            else if ((new_score - current_score) < temp * log_table[fast_rand() & 65535]) accept = true;
            
            if (!accept) { 
                tmp = current_T[w];
                current_T[w] = current_T[v];
                current_T[v] = current_T[u];
                current_T[u] = tmp;
            }
        }

        if (accept) {
            current_score = new_score;
            if (current_score < best_score) {
                best_score = current_score;
                for (int k = 0; k < 2 * n; k++) best_T[k] = current_T[k];
            }
        }
    }

    vector<int> final_P(2 * n);
    for (int i = 0; i < n; i++) {
        final_P[2 * i] = best_T[2 * i] * 2;
        final_P[2 * i + 1] = best_T[2 * i + 1] * 2;
    }

    vector<vector<int>> routing(n);
    int y_curr = 2; 
    vector<vector<bool>> row_used(100, vector<bool>(n, false));
    
    auto try_place = [&](int r, int c1, int c2) {
        int mn = min(c1, c2), mx = max(c1, c2);
        for (int c = mn; c <= mx; ++c) if (row_used[r][c]) return false;
        return true;
    };
    
    auto place = [&](int r, int c1, int c2) {
        int mn = min(c1, c2), mx = max(c1, c2);
        for (int c = mn; c <= mx; ++c) row_used[r][c] = true;
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
    
    auto check_parity = [&](int b_idx, int arr_p) {
        if (temp_parity[b_idx] == arr_p) {
            need_fix = true;
            temp_parity[b_idx] = 1 - arr_p;
        } else {
            temp_parity[b_idx] = arr_p;
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