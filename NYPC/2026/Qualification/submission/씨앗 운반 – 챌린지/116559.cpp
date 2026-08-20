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
    vector<pair<ll, int>> sorted_a(n), sorted_b(n);
    for(int i=0; i<n; ++i) { sorted_a[i] = {a[i], i}; sorted_b[i] = {b[i], i}; }
    sort(sorted_a.begin(), sorted_a.end());
    sort(sorted_b.begin(), sorted_b.end());
    
    for(int i=0; i<n; ++i) {
        P[2 * sorted_a[i].second] = 2 * sorted_b[i].second;
        P[2 * sorted_a[i].second + 1] = 2 * sorted_b[i].second + 1;
    }

    vector<ll> received(n, 0);
    int split_count = 0;
    for (int i = 0; i < n; i++) {
        int b1 = P[2 * i] / 2;
        int b2 = P[2 * i + 1] / 2;
        if (b1 == b2) {
            received[b1] += a[i];
        } else {
            received[b1] += (a[i] + 1) / 2;
            received[b2] += a[i] / 2;
            split_count++;
        }
    }
    
    ll current_err = 0;
    for (int i = 0; i < n; i++) current_err += abs(received[i] - b[i]);
    ll current_score = current_err * 10000LL + split_count;

    ll best_score = current_score;
    vector<int> best_P = P;

    double start_time = get_time();
    mt19937 rng(1337);
    
    double start_temp = 100000.0;
    double end_temp = 0.1;
    double max_time = 1.95; 

    int iter = 0;
    double temp = start_temp;
    int unique_b[4];
    static uniform_real_distribution<double> dist(0.0, 1.0); 

    while (true) {
        if ((iter & 4095) == 0) {
            double current_time = get_time();
            if (current_time - start_time > max_time) break;
            double progress = (current_time - start_time) / max_time;
            temp = start_temp * pow(end_temp / start_temp, progress);
        }
        iter++;

        if (best_score == 0) break; 

        int u = rng() % (2 * n);
        int v = rng() % (2 * n);
        if (u == v) continue;
        
        int i1 = u / 2, i2 = v / 2;

        if (i1 == i2) { 
            if (a[i1] % 2 == 0) continue; 
            int old_b1 = P[2 * i1] / 2;
            int old_b2 = P[2 * i1 + 1] / 2;
            if (old_b1 == old_b2) continue;
            
            ll new_err = current_err;
            new_err -= abs(received[old_b1] - b[old_b1]);
            new_err -= abs(received[old_b2] - b[old_b2]);

            received[old_b1]--;
            received[old_b2]++;

            new_err += abs(received[old_b1] - b[old_b1]);
            new_err += abs(received[old_b2] - b[old_b2]);

            ll new_score = new_err * 10000LL + split_count;

            bool accept = (new_score < current_score);
            if (!accept) {
                if (exp((current_score - new_score) / temp) > dist(rng)) accept = true;
            }

            if (accept) {
                current_err = new_err;
                current_score = new_score;
                swap(P[u], P[v]);
                if (current_score < best_score) {
                    best_score = current_score;
                    best_P = P;
                }
            } else { 
                received[old_b1]++;
                received[old_b2]--;
            }
        } else { 
            int old_b1_i1 = P[2*i1]/2, old_b2_i1 = P[2*i1+1]/2;
            int old_b1_i2 = P[2*i2]/2, old_b2_i2 = P[2*i2+1]/2;

            int num_b = 0;
            auto add_unique = [&](int b_idx) {
                for(int j=0; j<num_b; ++j) if(unique_b[j] == b_idx) return;
                unique_b[num_b++] = b_idx;
            };
            add_unique(old_b1_i1); add_unique(old_b2_i1);
            add_unique(old_b1_i2); add_unique(old_b2_i2);

            ll new_err = current_err;
            for(int j=0; j<num_b; ++j) new_err -= abs(received[unique_b[j]] - b[unique_b[j]]);

            int old_split = split_count;
            if (old_b1_i1 == old_b2_i1) received[old_b1_i1] -= a[i1];
            else { received[old_b1_i1] -= (a[i1]+1)/2; received[old_b2_i1] -= a[i1]/2; split_count--; }

            if (old_b1_i2 == old_b2_i2) received[old_b1_i2] -= a[i2];
            else { received[old_b1_i2] -= (a[i2]+1)/2; received[old_b2_i2] -= a[i2]/2; split_count--; }

            swap(P[u], P[v]);
            int new_b1_i1 = P[2*i1]/2, new_b2_i1 = P[2*i1+1]/2;
            int new_b1_i2 = P[2*i2]/2, new_b2_i2 = P[2*i2+1]/2;

            add_unique(new_b1_i1); add_unique(new_b2_i1);
            add_unique(new_b1_i2); add_unique(new_b2_i2);

            if (new_b1_i1 == new_b2_i1) received[new_b1_i1] += a[i1];
            else { received[new_b1_i1] += (a[i1]+1)/2; received[new_b2_i1] += a[i1]/2; split_count++; }

            if (new_b1_i2 == new_b2_i2) received[new_b1_i2] += a[i2];
            else { received[new_b1_i2] += (a[i2]+1)/2; received[new_b2_i2] += a[i2]/2; split_count++; }

            for(int j=0; j<num_b; ++j) new_err += abs(received[unique_b[j]] - b[unique_b[j]]);
            ll new_score = new_err * 10000LL + split_count;

            bool accept = (new_score < current_score);
            if (!accept) {
                if (exp((current_score - new_score) / temp) > dist(rng)) accept = true;
            }

            if (accept) {
                current_err = new_err;
                current_score = new_score;
                if (current_score < best_score) {
                    best_score = current_score;
                    best_P = P;
                }
            } else { 
                if (new_b1_i1 == new_b2_i1) received[new_b1_i1] -= a[i1];
                else { received[new_b1_i1] -= (a[i1]+1)/2; received[new_b2_i1] -= a[i1]/2; }

                if (new_b1_i2 == new_b2_i2) received[new_b1_i2] -= a[i2];
                else { received[new_b1_i2] -= (a[i2]+1)/2; received[new_b2_i2] -= a[i2]/2; }

                swap(P[u], P[v]);
                split_count = old_split;

                if (old_b1_i1 == old_b2_i1) received[old_b1_i1] += a[i1];
                else { received[old_b1_i1] += (a[i1]+1)/2; received[old_b2_i1] += a[i1]/2; }

                if (old_b1_i2 == old_b2_i2) received[old_b1_i2] += a[i2];
                else { received[old_b1_i2] += (a[i2]+1)/2; received[old_b2_i2] += a[i2]/2; }
            }
        }
    }

    P = best_P;
    vector<vector<int>> routing(n);
    int y_curr = 2; 
    vector<vector<bool>> row_used(100, vector<bool>(n, false));    
    auto try_place_same = [&](int y, int i, int b1) {
        if (row_used[y-1][i]) return false;
        if (i != b1 && row_used[y-1][b1]) return false;
        return true;
    };
    auto place_same = [&](int y, int i, int b1) {
        row_used[y-1][i] = true;
        if (i != b1) row_used[y-1][b1] = true;
    };

    auto try_place_diff = [&](int y, int i, int b1, int b2, int col2) {
        if (row_used[y-1][i]) return false;
        if (row_used[y-1][col2]) return false;
        if (col2 != b2 && row_used[y-1][b2]) return false;
        
        if (row_used[y][i]) return false;
        if (i != b1 && row_used[y][b1]) return false;
        return true;
    };
    auto place_diff = [&](int y, int i, int b1, int b2, int col2) {
        row_used[y-1][i] = true;
        row_used[y-1][col2] = true;
        if (col2 != b2) row_used[y-1][b2] = true;
        
        row_used[y][i] = true;
        if (i != b1) row_used[y][b1] = true;
    };

    for (int i = 0; i < n; i++) {
        int b1 = P[2 * i] / 2;
        int b2 = P[2 * i + 1] / 2;
        if (b1 == b2) {
            int y = 2;
            while (!try_place_same(y, i, b1)) y++;
            place_same(y, i, b1);
            routing[i] = {y};
            y_curr = max(y_curr, y + 1);
        } else {
            int col2 = (i == n - 1) ? i - 1 : i + 1;
            int y = 2;
            while (!try_place_diff(y, i, b1, b2, col2)) y++;
            place_diff(y, i, b1, b2, col2);
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
    if (rows < n) {
        rows = n;
        y_burrow = rows + 1;
    }

    vector<vector<string>> g(rows, vector<string>(n, "X"));
    vector<int> burrow_parity(n, -1);

    auto add_delay = [&](int b_idx, int arr_parity, int base_row, int col) {
        if (burrow_parity[b_idx] == arr_parity) {
            g[base_row - 1][col] = jump(y_fix - base_row, "D");
            g[y_fix - 1][b_idx] = jump(y_burrow - y_fix, "D");
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