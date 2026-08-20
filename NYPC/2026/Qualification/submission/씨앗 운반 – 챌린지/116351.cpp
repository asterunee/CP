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

void solve() {
    int n, t_total, m; cin >> n >> t_total >> m;

    vector<ll> a(n), b(n);
    for (int i = 0; i < n; i++) cin >> a[i];
    for (int i = 0; i < n; i++) cin >> b[i];

    vector<int> P(2 * n);
    iota(P.begin(), P.end(), 0);
    
    vector<pair<ll, int>> sorted_a(n), sorted_b(n);
    for(int i=0; i<n; ++i) { sorted_a[i] = {a[i], i}; sorted_b[i] = {b[i], i}; }
    sort(sorted_a.begin(), sorted_a.end());
    sort(sorted_b.begin(), sorted_b.end());
    for(int i=0; i<n; ++i) {
        P[2 * sorted_a[i].second] = 2 * sorted_b[i].second;
        P[2 * sorted_a[i].second + 1] = 2 * sorted_b[i].second + 1;
    }

    auto calc_score = [&](const vector<int>& p) {
        vector<ll> received(n, 0);
        int split_count = 0;
        for (int i = 0; i < n; i++) {
            int b1 = p[2 * i] / 2;
            int b2 = p[2 * i + 1] / 2;
            if (b1 == b2) {
                received[b1] += a[i];
            } else {
                received[b1] += (a[i] + 1) / 2;
                received[b2] += a[i] / 2;
                split_count++;
            }
        }
        ll err = 0;
        for (int i = 0; i < n; i++) err += abs(received[i] - b[i]);
        return err * 1000 + split_count; 
    };

    ll best_score = calc_score(P);
    vector<int> best_P = P;

    double start_time = get_time();
    mt19937 rng(1337);
    double temp = 1e9;
    double cooling_rate = 0.99995;

    while (get_time() - start_time < 1.85) {
        if (best_score == 0) break; 
        
        int u = rng() % (2 * n);
        int v = rng() % (2 * n);
        if (u == v) continue;

        swap(P[u], P[v]);
        ll cur_score = calc_score(P);

        if (cur_score < best_score || exp((best_score - cur_score) / temp) > (double)rng() / rng.max()) {
            best_score = cur_score;
            best_P = P;
        } else {
            swap(P[u], P[v]); 
        }
        temp *= cooling_rate;
    }

    P = best_P;

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
        int b1 = P[2 * i] / 2;
        int b2 = P[2 * i + 1] / 2;
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
        int b1 = P[2 * i] / 2;
        int b2 = P[2 * i + 1] / 2;
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
        int b1 = P[2 * i] / 2;
        int b2 = P[2 * i + 1] / 2;

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