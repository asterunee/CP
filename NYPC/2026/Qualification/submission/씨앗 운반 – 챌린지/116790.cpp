#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <cctype>

using namespace std;
using ll = long long;

static const int MAX_C = 10;
static const int MAX_R = 30;
static const int MAX_CELL = MAX_R * MAX_C;
static const int MAX_ALL = (MAX_R + 1) * MAX_C;
static const int MAX_EDGE = MAX_CELL * 4;
static const int MAX_EDIT = 40;

struct FastRNG {
    uint32_t x;
    explicit FastRNG(uint32_t seed) : x(seed ? seed : 1u) {}
    uint32_t next() {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return x;
    }
    int mod(int n) { return int(next() % uint32_t(n)); }
};

static void shuffle_ints(vector<int>& a, FastRNG& rng) {
    for (int i = int(a.size()) - 1; i > 0; --i) {
        int j = rng.mod(i + 1);
        swap(a[i], a[j]);
    }
}

struct Deadline {
    chrono::steady_clock::time_point end;
    bool expired() const { return chrono::steady_clock::now() >= end; }
};

struct Board {
    int R = -1;
    vector<vector<string>> g;
};

struct SimResult {
    bool valid = false;
    ll cost = (1LL << 62);
    ll E = (1LL << 60);
    ll L = (1LL << 60);
    int D = INT_MAX;
};

static string jump_token(int d, char dir) {
    if (d <= 0) return "X";
    if (d == 1) return string(1, dir);
    return to_string(d) + dir;
}

struct AssignmentK {
    vector<int> dst;
    ll err = (1LL << 60);
};

static AssignmentK eval_assignment(const vector<ll>& A, const vector<ll>& B,
                                   const vector<int>& dst, int K) {
    const int n = int(A.size());
    ll got[MAX_C] = {};
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < K; ++k) {
            got[dst[K * i + k]] += (A[i] + K - 1 - k) / K;
        }
    }
    AssignmentK z;
    z.dst = dst;
    z.err = 0;
    for (int j = 0; j < n; ++j) z.err += llabs(got[j] - B[j]);
    return z;
}

static vector<AssignmentK> make_assignments(const vector<ll>& A, const vector<ll>& B,
                                            int K, const Deadline& deadline) {
    const int n = int(A.size());
    const int N = K * n;
    vector<int> value(N), chunks(N), slot(N), best_slot(N), dst(N);
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < K; ++k) value[K * i + k] = int((A[i] + K - 1 - k) / K);
    }

    FastRNG rng(712367u + uint32_t(n * 991 + K * 100003));
    vector<AssignmentK> answer;
    answer.reserve(8);
    ll global_best = (1LL << 60);

    const int repetitions = 20;
    const int local_iterations = 96;

    for (int rep = 0; rep < repetitions; ++rep) {
        if (deadline.expired()) break;

        for (int i = 0; i < N; ++i) chunks[i] = i;
        shuffle_ints(chunks, rng);
        stable_sort(chunks.begin(), chunks.end(), [&](int x, int y) {
            return value[x] > value[y];
        });

        int cnt[MAX_C] = {};
        ll sum[MAX_C] = {};
        fill(slot.begin(), slot.end(), -1);

        for (int x : chunks) {
            int bj = -1;
            ll best_score = -(1LL << 60);
            for (int j = 0; j < n; ++j) if (cnt[j] < K) {
                const ll deficit = B[j] - sum[j];
                const ll score = deficit * 1000
                               - llabs(sum[j] + value[x] - B[j])
                               + ll(rng.next() % 31u);
                if (score > best_score) {
                    best_score = score;
                    bj = j;
                }
            }
            slot[K * bj + cnt[bj]++] = x;
            sum[bj] += value[x];
        }

        ll current = 0;
        for (int j = 0; j < n; ++j) current += llabs(sum[j] - B[j]);
        ll local_best = current;
        best_slot = slot;

        int stagnant = 0;
        for (int it = 0; it < local_iterations; ++it) {
            if ((it & 255) == 0 && deadline.expired()) break;

            ll best_delta = 0;
            int bp = -1, bq = -1;
            for (int p = 0; p < N; ++p) {
                const int u = p / K;
                const int x = slot[p];
                for (int q = p + 1; q < N; ++q) {
                    const int v = q / K;
                    if (u == v) continue;
                    const int y = slot[q];
                    const ll before = llabs(sum[u] - B[u]) + llabs(sum[v] - B[v]);
                    const ll nu = sum[u] + value[y] - value[x];
                    const ll nv = sum[v] + value[x] - value[y];
                    const ll delta = llabs(nu - B[u]) + llabs(nv - B[v]) - before;
                    if (delta < best_delta) {
                        best_delta = delta;
                        bp = p;
                        bq = q;
                    }
                }
            }

            if (bp >= 0) {
                const int u = bp / K, v = bq / K;
                const int x = slot[bp], y = slot[bq];
                sum[u] += value[y] - value[x];
                sum[v] += value[x] - value[y];
                swap(slot[bp], slot[bq]);
                current += best_delta;
                stagnant = 0;
                if (current < local_best) {
                    local_best = current;
                    best_slot = slot;
                }
                continue;
            }

            bool moved = false;
            for (int trial = 0; trial < 256; ++trial) {
                const int p = rng.mod(N), q = rng.mod(N), r = rng.mod(N);
                const int u = p / K, v = q / K, w = r / K;
                if (p == q || q == r || p == r || u == v || v == w || u == w) continue;
                const int x = slot[p], y = slot[q], z = slot[r];
                const ll before = llabs(sum[u] - B[u]) + llabs(sum[v] - B[v]) + llabs(sum[w] - B[w]);
                const ll nu = sum[u] + value[z] - value[x];
                const ll nv = sum[v] + value[x] - value[y];
                const ll nw = sum[w] + value[y] - value[z];
                const ll delta = llabs(nu - B[u]) + llabs(nv - B[v]) + llabs(nw - B[w]) - before;
                if (delta < 0 || (stagnant < 3 && delta <= max<ll>(2, current / 2000))) {
                    slot[p] = z; slot[q] = x; slot[r] = y;
                    sum[u] = nu; sum[v] = nv; sum[w] = nw;
                    current += delta;
                    moved = true;
                    if (current < local_best) {
                        local_best = current;
                        best_slot = slot;
                    }
                    break;
                }
            }

            if (!moved) {
                if (++stagnant >= 4) break;
                const int p = rng.mod(N), q = rng.mod(N);
                if (p != q && p / K != q / K) {
                    const int u = p / K, v = q / K;
                    const int x = slot[p], y = slot[q];
                    const ll before = llabs(sum[u] - B[u]) + llabs(sum[v] - B[v]);
                    sum[u] += value[y] - value[x];
                    sum[v] += value[x] - value[y];
                    swap(slot[p], slot[q]);
                    current += llabs(sum[u] - B[u]) + llabs(sum[v] - B[v]) - before;
                }
            }
        }

        for (int p = 0; p < N; ++p) dst[best_slot[p]] = p / K;
        AssignmentK candidate = eval_assignment(A, B, dst, K);

        bool duplicate = false;
        for (const AssignmentK& old : answer) {
            if (old.dst == candidate.dst) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            answer.push_back(move(candidate));
            sort(answer.begin(), answer.end(), [](const AssignmentK& x, const AssignmentK& y) {
                return x.err < y.err;
            });
            if (answer.size() > 8) answer.resize(8);
        }

        if (!answer.empty()) global_best = min(global_best, answer[0].err);
        if (global_best == 0 && rep >= 3) break;
    }
    return answer;
}

struct RouteEdit {
    uint8_t r = 0, c = 0;
    uint8_t kind = 0; 
    uint8_t tr = 0, tc = 0;
    uint8_t k = 0;
    uint8_t dir[4] = {};
};

struct PlacementPlan {
    int score = INT_MAX;
    uint16_t occ[MAX_R] = {};
    uint8_t residue[MAX_C] = {};
    int edit_count = 0;
    RouteEdit edit[MAX_EDIT];
};

static void add_hamster_edit(PlacementPlan& p, int r, int c, int tr, int tc) {
    RouteEdit& e = p.edit[p.edit_count++];
    e.r = uint8_t(r);
    e.c = uint8_t(c);
    e.kind = 0;
    e.tr = uint8_t(tr);
    e.tc = uint8_t(tc);
}

static void add_squirrel_edit(PlacementPlan& p, int r, int c, const int* dirs, int K) {
    RouteEdit& e = p.edit[p.edit_count++];
    e.r = uint8_t(r);
    e.c = uint8_t(c);
    e.kind = 1;
    e.k = uint8_t(K);
    for (int i = 0; i < K; ++i) e.dir[i] = uint8_t(dirs[i]);
}

static void apply_plan(const PlacementPlan& p, int R, vector<vector<string>>& g) {
    static const char dc[4] = {'U', 'D', 'L', 'R'};
    for (int z = 0; z < p.edit_count; ++z) {
        const RouteEdit& e = p.edit[z];
        if (e.kind == 1) {
            string s;
            s.reserve(e.k);
            for (int i = 0; i < e.k; ++i) s.push_back(dc[e.dir[i]]);
            g[e.r][e.c] = move(s);
        } else {
            const int r = e.r;
            const int c = e.c;
            const int tr = e.tr;
            const int tc = e.tc;
            if (r == tr) {
                g[r][c] = jump_token(abs(tc - c), tc > c ? 'R' : 'L');
            } else {
                g[r][c] = jump_token(abs(tr - r), tr > r ? 'D' : 'U');
            }
        }
    }
}

static void insert_plan(PlacementPlan* top, int& count, int limit, const PlacementPlan& p) {
    if (count < limit) {
        top[count++] = p;
    } else if (p.score < top[count - 1].score) {
        top[count - 1] = p;
    } else {
        return;
    }
    for (int i = count - 1; i > 0 && top[i].score < top[i - 1].score; --i) {
        swap(top[i], top[i - 1]);
    }
}

static bool build_split_board(const vector<ll>& A, const vector<int>& dst,
                              int M, int R, int K, uint32_t seed,
                              const Deadline& deadline, Board& out) {
    const int n = int(A.size());
    FastRNG rng(seed);

    vector<int> order;
    order.reserve(n);
    for (int i = 0; i < n; ++i) if (A[i] > 0) order.push_back(i);
    shuffle_ints(order, rng);

    uint16_t occ[MAX_R] = {};
    uint8_t residue[MAX_C] = {};
    vector<vector<string>> g(R, vector<string>(n, "X"));
    for (int i : order) occ[0] |= uint16_t(1u << i);

    static const int dr[4] = {-1, 1, 0, 0};
    static const int dc[4] = {0, 0, -1, 1};

    for (int source : order) {
        if (deadline.expired()) return false;

        PlacementPlan top[10];
        int top_count = 0;

        vector<int> centers;
        if (K < 4) {
            centers.push_back(source);
        } else {
            for (int c = 1; c + 1 < n; ++c) {
                if (c == source || abs(c - source) >= 2) centers.push_back(c);
            }
            if (centers.empty()) return false;
            shuffle_ints(centers, rng);
        }

        for (int center : centers) {
            int avail[4], avail_count = 0;
            avail[avail_count++] = 0;
            avail[avail_count++] = 1;
            if (center > 0) avail[avail_count++] = 2;
            if (center + 1 < n) avail[avail_count++] = 3;
            if (avail_count < K) continue;

            int perm[4] = {};
            bool used_dir[4] = {};

            for (int y = 2; y <= R - 1; ++y) {
                auto evaluate = [&]() {
                    PlacementPlan p;
                    memcpy(p.occ, occ, sizeof(occ));
                    memcpy(p.residue, residue, sizeof(residue));
                    p.edit_count = 0;
                    p.score = y + (center != source) * 8;

                    const uint16_t center_bit = uint16_t(1u << center);
                    if (p.occ[y - 1] & center_bit) return;
                    p.occ[y - 1] |= center_bit;

                    const int input_extra = (center != source);
                    if (input_extra) {
                        const uint16_t source_bit = uint16_t(1u << source);
                        if (p.occ[y - 1] & source_bit) return;
                        p.occ[y - 1] |= source_bit;
                        add_hamster_edit(p, y - 1, source, y - 1, center);
                    }

                    for (int k = 0; k < K; ++k) {
                        if ((A[source] + K - 1 - k) / K == 0) continue;

                        const int d = perm[k];
                        const int lr = y + dr[d];
                        const int lc = center + dc[d];
                        if (lr < 1 || lr > R || lc < 0 || lc >= n) return;

                        const uint16_t lbit = uint16_t(1u << lc);
                        if (p.occ[lr - 1] & lbit) return;
                        p.occ[lr - 1] |= lbit;

                        const int b = dst[K * source + k];
                        const uint16_t bbit = uint16_t(1u << b);
                        if (lc != b) {
                            if (p.occ[lr - 1] & bbit) return;
                            p.occ[lr - 1] |= bbit;
                            add_hamster_edit(p, lr - 1, lc, lr - 1, b);
                        }

                        const int base = int(((M - A[source] + 1) + k + 2
                                             + input_extra + (lc != b)) % K);
                        int best_delay = -1;
                        int best_residue = -1;
                        int best_rows[4] = {};

                        for (int want = 0; want < K; ++want) {
                            if (p.residue[b] & (1u << want)) continue;
                            const int delay = (want - base + K) % K;
                            int free_rows[MAX_R], free_count = 0;
                            for (int rr = 1; rr <= R; ++rr) {
                                if ((p.occ[rr - 1] & bbit) == 0) free_rows[free_count++] = rr;
                            }
                            if (free_count < delay) continue;

                            if (best_delay < 0 || delay < best_delay) {
                                best_delay = delay;
                                best_residue = want;
                                if (delay > 0) {
                                    const bool reverse_order = (rng.next() & 1u) != 0;
                                    for (int z = 0; z < delay; ++z) {
                                        best_rows[z] = reverse_order ? free_rows[free_count - 1 - z] : free_rows[z];
                                    }
                                }
                            }
                        }
                        if (best_delay < 0) return;

                        int cr = lr;
                        for (int z = 0; z < best_delay; ++z) {
                            const int q = best_rows[z];
                            add_hamster_edit(p, cr - 1, b, q - 1, b);
                            p.occ[q - 1] |= bbit;
                            cr = q;
                        }
                        add_hamster_edit(p, cr - 1, b, R, b);
                        p.residue[b] |= uint8_t(1u << best_residue);
                        p.score += best_delay * 20 + (lc != b) * 3 + abs(lc - b);
                    }

                    add_squirrel_edit(p, y - 1, center, perm, K);
                    add_hamster_edit(p, 0, source, y - 1, source);
                    insert_plan(top, top_count, 10, p);
                };

                auto dfs = [&](auto&& self, int depth) -> void {
                    if (depth == K) {
                        evaluate();
                        return;
                    }
                    for (int z = 0; z < avail_count; ++z) {
                        const int d = avail[z];
                        if (used_dir[d]) continue;
                        used_dir[d] = true;
                        perm[depth] = d;
                        self(self, depth + 1);
                        used_dir[d] = false;
                    }
                };
                dfs(dfs, 0);
            }
        }

        if (top_count == 0) return false;
        const int pick_limit = min(6, top_count);
        const int pick = rng.mod(pick_limit);
        memcpy(occ, top[pick].occ, sizeof(occ));
        memcpy(residue, top[pick].residue, sizeof(residue));
        apply_plan(top[pick], R, g);
    }

    out.R = R;
    out.g = move(g);
    return true;
}

static vector<int> best_permutation(const vector<ll>& A, const vector<ll>& B) {
    const int n = int(A.size());
    const int S = 1 << n;
    const int INF = 1000000000;
    vector<int> dp(S, INF), parent(S, -1);
    dp[0] = 0;
    for (int mask = 0; mask < S; ++mask) {
        if (dp[mask] == INF) continue;
        const int i = __builtin_popcount(unsigned(mask));
        if (i == n) continue;
        for (int j = 0; j < n; ++j) if ((mask & (1 << j)) == 0) {
            const int nm = mask | (1 << j);
            const int nv = dp[mask] + int(llabs(A[i] - B[j]));
            if (nv < dp[nm]) {
                dp[nm] = nv;
                parent[nm] = j;
            }
        }
    }

    vector<int> to(n);
    int mask = S - 1;
    for (int i = n - 1; i >= 0; --i) {
        int j = parent[mask];
        if (j < 0) j = i;
        to[i] = j;
        mask ^= 1 << j;
    }
    return to;
}

static bool build_permutation_board(const vector<ll>& A, const vector<int>& to,
                                    int R, uint32_t seed, Board& out) {
    const int n = int(A.size());
    FastRNG rng(seed);
    vector<int> order;
    for (int i = 0; i < n; ++i) if (A[i] > 0 && to[i] != i) order.push_back(i);
    shuffle_ints(order, rng);

    uint16_t used[MAX_R] = {};
    vector<vector<string>> g(R, vector<string>(n, "X"));
    for (int i = 0; i < n; ++i) {
        if (A[i] == 0) continue;
        used[0] |= uint16_t(1u << i);
        if (to[i] == i) g[0][i] = jump_token(R, 'D');
    }

    for (int i : order) {
        const int b = to[i];
        int rows[MAX_R], count = 0;
        const uint16_t mask = uint16_t((1u << i) | (1u << b));
        for (int r = 2; r <= R; ++r) {
            if ((used[r - 1] & mask) == 0) rows[count++] = r;
        }
        if (count == 0) return false;
        const int r = rows[rng.mod(min(3, count))];
        used[r - 1] |= mask;
        g[0][i] = jump_token(r - 1, 'D');
        g[r - 1][i] = jump_token(abs(b - i), b > i ? 'R' : 'L');
        g[r - 1][b] = jump_token(R + 1 - r, 'D');
    }

    out.R = R;
    out.g = move(g);
    return true;
}

static Board direct_board(int n) {
    Board b;
    b.R = n;
    b.g.assign(n, vector<string>(n, "X"));
    for (int c = 0; c < n; ++c) b.g[0][c] = jump_token(n, 'D');
    return b;
}

struct ParsedCell {
    uint8_t k = 0;
    int8_t dr[4] = {};
    int8_t dc[4] = {};
    uint8_t len[4] = {};
};

static SimResult simulate_board(const vector<ll>& A, const vector<ll>& B,
                                int T, int M, const Board& board) {
    const int C = int(A.size());
    const int R = board.R;
    SimResult res;
    if (R < C || R > C + 20 || int(board.g.size()) != R) return res;

    ParsedCell cell[MAX_CELL];
    int active[MAX_CELL], active_count = 0;
    const int grid_cells = R * C;
    memset(cell, 0, sizeof(cell));

    auto direction = [](char ch, int& dr, int& dc) -> bool {
        if (ch == 'U') { dr = -1; dc = 0; return true; }
        if (ch == 'D') { dr = 1; dc = 0; return true; }
        if (ch == 'L') { dr = 0; dc = -1; return true; }
        if (ch == 'R') { dr = 0; dc = 1; return true; }
        return false;
    };

    for (int r = 0; r < R; ++r) {
        if (int(board.g[r].size()) != C) return res;
        for (int c = 0; c < C; ++c) {
            const string& s = board.g[r][c];
            ParsedCell& pc = cell[r * C + c];
            if (s == "X") continue;

            if (isdigit(static_cast<unsigned char>(s[0]))) {
                int p = 0, d = 0;
                while (p < int(s.size()) && isdigit(static_cast<unsigned char>(s[p]))) {
                    d = d * 10 + (s[p] - '0');
                    ++p;
                }
                if (d <= 0 || p + 1 != int(s.size())) return res;
                int dr = 0, dc = 0;
                if (!direction(s[p], dr, dc)) return res;
                pc.k = 1;
                pc.dr[0] = int8_t(dr);
                pc.dc[0] = int8_t(dc);
                pc.len[0] = uint8_t(d);
            } else {
                if (s.empty() || s.size() > 4) return res;
                bool seen[4] = {};
                pc.k = uint8_t(s.size());
                for (int z = 0; z < int(s.size()); ++z) {
                    const char ch = s[z];
                    const int id = (ch == 'U' ? 0 : ch == 'D' ? 1 : ch == 'L' ? 2 : ch == 'R' ? 3 : -1);
                    if (id < 0 || seen[id]) return res;
                    seen[id] = true;
                    int dr = 0, dc = 0;
                    direction(ch, dr, dc);
                    pc.dr[z] = int8_t(dr);
                    pc.dc[z] = int8_t(dc);
                    pc.len[z] = 1;
                }
            }
            active[active_count++] = r * C + c;
        }
    }

    bool sparse_safe = true;
    for (int z = 0; z < active_count; ++z) {
        const int id = active[z];
        const int r = id / C;
        const int c = id % C;
        const ParsedCell& pc = cell[id];
        for (int e = 0; e < pc.k; ++e) {
            const int nr = r + int(pc.dr[e]) * pc.len[e];
            const int nc = c + int(pc.dc[e]) * pc.len[e];
            if (nr < 0 || nr > R || nc < 0 || nc >= C) return res;
            if (nr < R && cell[nr * C + nc].k == 0) sparse_safe = false;
        }
    }

    int scan_ids[MAX_CELL], scan_count = 0;
    if (sparse_safe) {
        memcpy(scan_ids, active, sizeof(int) * active_count);
        scan_count = active_count;
    } else {
        for (int id = 0; id < grid_cells; ++id) scan_ids[scan_count++] = id;
    }

    int q[MAX_ALL] = {};
    uint8_t ptr[MAX_CELL] = {};
    uint8_t over[MAX_ALL] = {};
    int from[MAX_EDGE], to[MAX_EDGE];
    int stored[MAX_C] = {};
    int start[MAX_C];
    for (int c = 0; c < C; ++c) start[c] = M - int(A[c]) + 1;

    int last = 0;
    int total_in_system = 0;

    for (int t = 1; t <= T; ++t) {
        if (t <= M) {
            for (int c = 0; c < C; ++c) {
                if (t >= start[c]) {
                    ++q[c];
                    ++total_in_system;
                }
            }
        }

        memset(over, 0, size_t((R + 1) * C));
        int edge_count = 0;

        for (int z = 0; z < scan_count; ++z) {
            const int id = scan_ids[z];
            const int have = q[id];
            if (have == 0) continue;
            const ParsedCell& pc = cell[id];
            if (pc.k == 0) {
                over[id] = 1;
                continue;
            }

            const int send = have < pc.k ? have : pc.k;
            if (have > pc.k) over[id] = 1;
            q[id] -= send;

            const int r = id / C;
            const int c = id - r * C;
            int p = ptr[id];
            for (int ecount = 0; ecount < send; ++ecount) {
                int e = p + ecount;
                if (e >= pc.k) e -= pc.k;
                const int nr = r + int(pc.dr[e]) * pc.len[e];
                const int nc = c + int(pc.dc[e]) * pc.len[e];
                from[edge_count] = id;
                to[edge_count] = nr * C + nc;
                ++edge_count;
            }
            p += send;
            if (p >= pc.k) p -= pc.k;
            ptr[id] = uint8_t(p);
        }

        const int burrow_base = R * C;
        for (int c = 0; c < C; ++c) {
            if (q[burrow_base + c] > 0) over[burrow_base + c] = 1;
        }

        for (int z = 0; z < edge_count; ++z) {
            if (over[to[z]]) ++q[from[z]];
            else ++q[to[z]];
        }

        for (int c = 0; c < C; ++c) {
            const int id = burrow_base + c;
            if (q[id] > 0) {
                --q[id];
                ++stored[c];
                --total_in_system;
                last = t;
            }
        }

        if (t >= M && total_in_system == 0) break;
    }

    ll E = 0, need = 0, done = 0;
    for (int c = 0; c < C; ++c) {
        E += llabs(ll(stored[c]) - B[c]);
        need += B[c];
        done += stored[c];
    }
    const ll L = need - done;
    const int D = (L > 0 ? T : last - M);
    const ll row_cost = 1LL << (R - C);

    res.valid = true;
    res.E = E;
    res.L = L;
    res.D = D;
    res.cost = row_cost + max<ll>(E, D) + ll(T) * L;
    return res;
}

static bool better_result(const SimResult& x, const Board& bx,
                          const SimResult& y, const Board& by) {
    if (!x.valid) return false;
    if (!y.valid) return true;
    if (x.cost != y.cost) return x.cost < y.cost;
    if (x.L != y.L) return x.L < y.L;
    if (x.E != y.E) return x.E < y.E;
    if (x.D != y.D) return x.D < y.D;
    return bx.R < by.R;
}

struct Phase {
    int K = 0;
    vector<AssignmentK> candidates;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C, T, M;
    if (!(cin >> C >> T >> M)) return 0;
    vector<ll> A(C), B(C);
    for (ll& x : A) cin >> x;
    for (ll& x : B) cin >> x;

    const auto begin = chrono::steady_clock::now();
    const Deadline deadline{begin + chrono::milliseconds(900)};

    const bool exact_simulation = (T <= 10000);

    Board best_board = direct_board(C);
    SimResult best_result;
    if (exact_simulation) {
        best_result = simulate_board(A, B, T, M, best_board);
    } else {
        best_result.valid = true;
        best_result.E = 0;
        for (int i = 0; i < C; ++i) best_result.E += llabs(A[i] - B[i]);
        best_result.L = 0;
        best_result.D = 0;
        best_result.cost = 1 + best_result.E;
    }

    uint32_t base_seed = 0x9E3779B9u;
    for (ll x : A) base_seed = base_seed * 1664525u + uint32_t(x) + 1013904223u;
    for (ll x : B) base_seed = base_seed * 1664525u + uint32_t(x) + 1013904223u;

    const vector<int> permutation = best_permutation(A, B);
    ll permutation_error = 0;
    for (int i = 0; i < C; ++i) permutation_error += llabs(A[i] - B[permutation[i]]);
    for (int rr = C; rr <= min(C + 2, C + 20) && !deadline.expired(); ++rr) {
        for (int v = 0; v < 8 && !deadline.expired(); ++v) {
            Board candidate;
            if (!build_permutation_board(A, permutation, rr,
                    base_seed ^ uint32_t(0xA17E000u + rr * 131 + v), candidate)) continue;
            SimResult result;
            if (exact_simulation) {
                result = simulate_board(A, B, T, M, candidate);
            } else {
                result.valid = true;
                result.E = permutation_error;
                result.L = 0;
                result.D = 3;
                result.cost = (1LL << (rr - C)) + max<ll>(result.E, result.D);
            }
            if (better_result(result, candidate, best_result, best_board)) {
                best_result = result;
                best_board = move(candidate);
            }
        }
    }

    vector<Phase> phases;
    phases.reserve(3);
    for (int K : {4, 3, 2}) {
        if (deadline.expired()) break;
        if (K == 4 && C < 3) continue;
        Phase p;
        p.K = K;
        p.candidates = make_assignments(A, B, K, deadline);
        if (!p.candidates.empty()) phases.push_back(move(p));
    }

    sort(phases.begin(), phases.end(), [](const Phase& x, const Phase& y) {
        return x.candidates[0].err < y.candidates[0].err;
    });

    for (const Phase& phase : phases) {
        if (deadline.expired()) break;
        const int K = phase.K;
        if (1 + phase.candidates[0].err >= best_result.cost) continue;

        const int keep = (K == 4 ? 6 : K == 3 ? 5 : 4);
        const int variants = (K == 4 ? 28 : K == 3 ? 20 : 14);
        const int extra_rows = (K == 4 ? 15 : K == 3 ? 10 : 6);

        for (int ai = 0; ai < int(phase.candidates.size()) && ai < keep; ++ai) {
            if (deadline.expired()) break;
            if (1 + phase.candidates[ai].err >= best_result.cost) break;

            vector<int> row_order;
            row_order.reserve(extra_rows + 1);
            if (K == 4) {
                for (int d = 7; d <= extra_rows; ++d) row_order.push_back(C + d);
                for (int d = 0; d < 7; ++d) row_order.push_back(C + d);
            } else {
                for (int d = 0; d <= extra_rows; ++d) row_order.push_back(C + d);
            }

            for (int rr : row_order) {
                if (deadline.expired()) break;
                if (rr > C + 20) continue;
                const ll row_lower_bound = 1LL << (rr - C);
                if (row_lower_bound + phase.candidates[ai].err >= best_result.cost) continue;

                bool found_for_row = false;
                for (int v = 0; v < variants; ++v) {
                    if ((v & 3) == 0 && deadline.expired()) break;
                    Board candidate;
                    const uint32_t seed = base_seed ^ uint32_t(
                        K * 0x51ED000u + ai * 10007 + rr * 257 + v * 65537u);
                    if (!build_split_board(A, phase.candidates[ai].dst, M, rr, K,
                                           seed, deadline, candidate)) continue;

                    SimResult result;
                    if (exact_simulation) {
                        result = simulate_board(A, B, T, M, candidate);
                    } else {
                        result.valid = true;
                        result.E = phase.candidates[ai].err;
                        result.L = 0;
                        result.D = K + 5;
                        result.cost = row_lower_bound + max<ll>(result.E, result.D);
                    }
                    if (better_result(result, candidate, best_result, best_board)) {
                        best_result = result;
                        best_board = move(candidate);
                    }
                    found_for_row = true;
                    if (!exact_simulation) break;
                }
                if (found_for_row && !exact_simulation) break;
            }
        }
    }

    cout << best_board.R << '\n';
    for (int r = 0; r < best_board.R; ++r) {
        for (int c = 0; c < C; ++c) {
            if (c) cout << ' ';
            cout << best_board.g[r][c];
        }
        cout << '\n';
    }
    return 0;
}
