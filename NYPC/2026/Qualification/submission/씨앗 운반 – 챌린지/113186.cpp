#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>

using namespace std;

int C, T, M;
int A[15], B[15];

uint32_t rnd_seed = 123456789;
inline uint32_t fast_rand() {
    rnd_seed ^= rnd_seed << 13;
    rnd_seed ^= rnd_seed >> 17;
    rnd_seed ^= rnd_seed << 5;
    return rnd_seed;
}

struct State {
    int R;
    int type[400];
    int p_len[400];
    int p_dirs[400][4];
    int ham_target[400];
    int adj[400][4];
};

int dr[] = {-1, 1, 0, 0}; 
int dc[] = {0, 0, -1, 1}; 
char dir_chars[] = {'U', 'D', 'L', 'R'};

vector<vector<int>> valid_sq[400];
vector<int> valid_ham[400];

int grid_seeds[400];
int in_cnt[400];
int in_src[400][16];
int sq_idx[400];

int active[400];
bool in_active[400];
int active_targets[400];
bool in_targets[400];
bool overloaded[400];
int sent_cnt[400];
int next_active[400];
bool next_in_active[400];

int req_total = 0;

int simulate(const State& S) {
    int total_cells = (S.R + 2) * C;
    int stored[15] = {0};
    int total_stored = 0;
    int last_store = 0;

    memset(grid_seeds, 0, total_cells * sizeof(int));
    memset(sq_idx, 0, total_cells * sizeof(int));
    memset(in_active, 0, total_cells * sizeof(bool));
    memset(in_targets, 0, total_cells * sizeof(bool));
    
    int num_active = 0;
    int num_targets = 0;

    for (int t = 1; t <= T; ++t) {
        for (int i = 0; i < C; ++i) {
            if (t >= M - A[i] + 1 && t <= M) {
                int idx = C + i;
                grid_seeds[idx]++;
                if (!in_active[idx]) {
                    in_active[idx] = true;
                    active[num_active++] = idx;
                }
            }
        }

        memset(in_cnt, 0, total_cells * sizeof(int));
        memset(overloaded, 0, total_cells * sizeof(bool));
        memset(sent_cnt, 0, total_cells * sizeof(int));

        for (int i = 0; i < num_active; ++i) {
            int idx = active[i];
            int seeds = grid_seeds[idx];
            if (seeds == 0) continue;

            if (idx < (S.R + 1) * C) { 
                if (S.type[idx] == 1) { 
                    int p_l = S.p_len[idx];
                    int to_send = min(seeds, p_l);
                    for (int k = 0; k < to_send; ++k) {
                        int dir = S.p_dirs[idx][sq_idx[idx]];
                        sq_idx[idx] = (sq_idx[idx] + 1) % p_l;
                        int target = S.adj[idx][dir];
                        in_src[target][in_cnt[target]++] = idx;
                        if (!in_targets[target]) {
                            in_targets[target] = true;
                            active_targets[num_targets++] = target;
                        }
                    }
                    sent_cnt[idx] = to_send;
                } else if (S.type[idx] == 2) {
                    int target = S.ham_target[idx];
                    in_src[target][in_cnt[target]++] = idx;
                    if (!in_targets[target]) {
                        in_targets[target] = true;
                        active_targets[num_targets++] = target;
                    }
                    sent_cnt[idx] = 1;
                }
            }
            
            if (seeds > sent_cnt[idx]) overloaded[idx] = true;
            grid_seeds[idx] -= sent_cnt[idx];
            
            if (!in_targets[idx]) { 
                in_targets[idx] = true; 
                active_targets[num_targets++] = idx; 
            }
        }

        int num_next = 0;
        memset(next_in_active, 0, total_cells * sizeof(bool));

        for (int i = 0; i < num_targets; ++i) {
            int idx = active_targets[i];
            if (in_cnt[idx] > 0) {
                if (overloaded[idx]) {
                    for (int k = 0; k < in_cnt[idx]; ++k) {
                        int sender = in_src[idx][k];
                        grid_seeds[sender]++; 
                        if (!in_targets[sender]) { 
                            in_targets[sender] = true; 
                            active_targets[num_targets++] = sender; 
                        }
                    }
                } else {
                    grid_seeds[idx] += in_cnt[idx]; 
                }
            }
        }

        for (int i = 0; i < num_targets; ++i) {
            int idx = active_targets[i];
            if (idx >= (S.R + 1) * C) { 
                if (grid_seeds[idx] > 0) {
                    grid_seeds[idx]--;
                    int col = idx % C;
                    stored[col]++;
                    total_stored++;
                    last_store = t;
                }
            }
            if (grid_seeds[idx] > 0) {
                if (!next_in_active[idx]) {
                    next_in_active[idx] = true;
                    next_active[num_next++] = idx;
                }
            }
            in_targets[idx] = false; 
        }
        num_targets = 0;

        num_active = num_next;
        memcpy(active, next_active, num_next * sizeof(int));
        memcpy(in_active, next_in_active, total_cells * sizeof(bool));

        if (total_stored == req_total) break; 
    }

    int E = 0;
    int L = req_total - total_stored;
    for (int i = 0; i < C; ++i) E += abs(stored[i] - B[i]);
    
    int D = (L > 0) ? T : max(0, last_store - M);
    return 2 * S.R - C + max(E, D) + T * L;
}

string format_cell(int idx, const State& S) {
    if (S.type[idx] == 0) return "X";
    if (S.type[idx] == 1) {
        string res = "";
        for (int i = 0; i < S.p_len[idx]; ++i) res += dir_chars[S.p_dirs[idx][i]];
        return res;
    }
    if (S.type[idx] == 2) {
        int r = idx / C;
        int c = idx % C;
        int tr = S.ham_target[idx] / C;
        int tc = S.ham_target[idx] % C;
        if (tr > r) return to_string(tr - r) + "D";
        if (tr < r) return to_string(r - tr) + "U";
        if (tc > c) return to_string(tc - c) + "R";
        if (tc < c) return to_string(c - tc) + "L";
    }
    return "X";
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> C >> T >> M)) return 0;
    for (int i = 0; i < C; ++i) cin >> A[i];
    for (int i = 0; i < C; ++i) {
        cin >> B[i];
        req_total += B[i];
    }

    auto start_time = chrono::steady_clock::now();
    double time_limit = 1.95;

    State best_overall;
    int best_cost_overall = 2e9;

    for (int current_R = C; current_R <= C + 2; ++current_R) {
        State S;
        S.R = current_R;

        for (int idx = C; idx < (current_R + 1) * C; ++idx) {
            int r = idx / C;
            int c = idx % C;
            
            vector<int> ok_dirs;
            for (int d = 0; d < 4; ++d) {
                int nr = r + dr[d];
                int nc = c + dc[d];
                if (nr > 0 && nr <= current_R + 1 && nc >= 0 && nc < C) {
                    S.adj[idx][d] = nr * C + nc;
                    ok_dirs.push_back(d);
                } else {
                    S.adj[idx][d] = -1;
                }
            }
            
            valid_sq[idx].clear();
            for (int mask = 1; mask < (1 << ok_dirs.size()); ++mask) {
                vector<int> subset;
                for (int i = 0; i < ok_dirs.size(); ++i) {
                    if (mask & (1 << i)) subset.push_back(ok_dirs[i]);
                }
                do { valid_sq[idx].push_back(subset); } 
                while (next_permutation(subset.begin(), subset.end()));
            }

            valid_ham[idx].clear();
            for (int nr = 1; nr <= current_R + 1; ++nr) {
                if (abs(nr - r) >= 2) valid_ham[idx].push_back(nr * C + c);
            }
            for (int nc = 0; nc < C; ++nc) {
                if (abs(nc - c) >= 2) valid_ham[idx].push_back(r * C + nc);
            }
        }

        for (int idx = C; idx < (current_R + 1) * C; ++idx) {
            S.type[idx] = 1;
            S.p_len[idx] = 1;
            S.p_dirs[idx][0] = 1; 
        }

        int current_cost = simulate(S);
        State best_for_R = S;
        int best_cost_R = current_cost;

        double T_start = 500.0;
        double T_end = 0.1;
        int iter = 0;
        
        double allocated = (current_R == C) ? 1.1 : ((current_R == C + 1) ? 1.6 : 1.95);

        while (true) {
            if ((iter & 2047) == 0) {
                auto now = chrono::steady_clock::now();
                double elapsed = chrono::duration<double>(now - start_time).count();
                if (elapsed > time_limit || elapsed > allocated) break;
            }
            iter++;

            State next_S = S;
            int idx = C + fast_rand() % (current_R * C);
            int choice = fast_rand() % 100;

            if (choice < 15) {
                next_S.type[idx] = 0;
            } else if (choice < 55 && !valid_ham[idx].empty()) {
                next_S.type[idx] = 2;
                next_S.ham_target[idx] = valid_ham[idx][fast_rand() % valid_ham[idx].size()];
            } else {
                next_S.type[idx] = 1;
                const auto& pats = valid_sq[idx];
                int p_idx = fast_rand() % pats.size();
                next_S.p_len[idx] = pats[p_idx].size();
                for (int i = 0; i < next_S.p_len[idx]; ++i) {
                    next_S.p_dirs[idx][i] = pats[p_idx][i];
                }
            }

            int next_cost = simulate(next_S);
            int delta = next_cost - current_cost;

            double progress = chrono::duration<double>(chrono::steady_clock::now() - start_time).count() / allocated;
            double temp = T_start * pow(T_end / T_start, min(1.0, progress));

            if (delta <= 0 || (double)(fast_rand() % 1000000) / 1000000.0 < exp(-delta / temp)) {
                S = next_S;
                current_cost = next_cost;
                if (current_cost < best_cost_R) {
                    best_cost_R = current_cost;
                    best_for_R = S;
                    if (best_cost_R == 2 * current_R - C) break; 
                }
            }
        }

        if (best_cost_R < best_cost_overall) {
            best_cost_overall = best_cost_R;
            best_overall = best_for_R;
        }

        if (best_cost_overall == 2 * current_R - C) break;
    }

    cout << best_overall.R << "\n";
    for (int r = 1; r <= best_overall.R; ++r) {
        for (int c = 0; c < C; ++c) {
            cout << format_cell(r * C + c, best_overall) << (c == C - 1 ? "" : " ");
        }
        cout << "\n";
    }

    return 0;
}