#include <bits/stdc++.h>
#pragma GCC optimize("O3,unroll-loops")
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


static vector<AssignmentK> exact_k4_c5(const vector<ll>& A, const vector<ll>& B,
                                       const Deadline& deadline) {
    if (A.size() != 5) return {};
    constexpr int N = 20;
    constexpr int S = 1 << N;
    constexpr int INF = 1000000000;
    int value[N];
    for (int i = 0; i < 5; ++i)
        for (int k = 0; k < 4; ++k)
            value[4 * i + k] = int((A[i] + 3 - k) / 4);

    struct Group { uint32_t mask; int sum; };
    vector<Group> groups;
    groups.reserve(4845);
    for (uint32_t m = 0; m < (1u << N); ++m) {
        if (__builtin_popcount(m) != 4) continue;
        int sum = 0;
        uint32_t q = m;
        while (q) {
            const int b = __builtin_ctz(q);
            sum += value[b];
            q &= q - 1;
        }
        groups.push_back({m, sum});
    }

    vector<int> p01(S, INF), p23(S, INF);
    const int G = int(groups.size());
    for (int x = 0; x < G; ++x) {
        if ((x & 255) == 0 && deadline.expired()) return {};
        for (int y = x + 1; y < G; ++y) {
            if (groups[x].mask & groups[y].mask) continue;
            const uint32_t m = groups[x].mask | groups[y].mask;
            int z = abs(groups[x].sum - int(B[0])) + abs(groups[y].sum - int(B[1]));
            z = min(z, abs(groups[y].sum - int(B[0])) + abs(groups[x].sum - int(B[1])));
            if (z < p01[m]) p01[m] = z;
        }
    }
    for (int x = 0; x < G; ++x) {
        if ((x & 255) == 0 && deadline.expired()) return {};
        for (int y = x + 1; y < G; ++y) {
            if (groups[x].mask & groups[y].mask) continue;
            const uint32_t m = groups[x].mask | groups[y].mask;
            int z = abs(groups[x].sum - int(B[2])) + abs(groups[y].sum - int(B[3]));
            z = min(z, abs(groups[y].sum - int(B[2])) + abs(groups[x].sum - int(B[3])));
            if (z < p23[m]) p23[m] = z;
        }
    }

    const uint32_t ALL = (1u << N) - 1;
    int best = INF;
    uint32_t best01 = 0, best4 = 0;
    for (uint32_t m01 = 0; m01 <= ALL; ++m01) {
        if (p01[m01] == INF || __builtin_popcount(m01) != 8) continue;
        const uint32_t rem = ALL ^ m01;
        int bit[12], cnt = 0;
        uint32_t q = rem;
        while (q) {
            bit[cnt++] = __builtin_ctz(q);
            q &= q - 1;
        }
        for (int a = 0; a < 9; ++a)
        for (int b = a + 1; b < 10; ++b)
        for (int c = b + 1; c < 11; ++c)
        for (int d = c + 1; d < 12; ++d) {
            const uint32_t m4 = (1u << bit[a]) | (1u << bit[b]) |
                                (1u << bit[c]) | (1u << bit[d]);
            const uint32_t m23 = rem ^ m4;
            if (p23[m23] == INF) continue;
            const int s4 = value[bit[a]] + value[bit[b]] + value[bit[c]] + value[bit[d]];
            const int z = p01[m01] + p23[m23] + abs(s4 - int(B[4]));
            if (z < best) {
                best = z;
                best01 = m01;
                best4 = m4;
            }
        }
    }
    if (best == INF || deadline.expired()) return {};

    auto recover = [&](uint32_t mask8, int t0, int t1, int wanted,
                       uint32_t& g0, uint32_t& g1) {
        for (const Group& x : groups) {
            if ((x.mask & mask8) != x.mask) continue;
            const uint32_t ym = mask8 ^ x.mask;
            if (__builtin_popcount(ym) != 4) continue;
            int sy = 0;
            uint32_t q2 = ym;
            while (q2) {
                const int b = __builtin_ctz(q2);
                sy += value[b];
                q2 &= q2 - 1;
            }
            if (abs(x.sum - int(B[t0])) + abs(sy - int(B[t1])) == wanted) {
                g0 = x.mask; g1 = ym; return true;
            }
        }
        return false;
    };

    const uint32_t rem = ALL ^ best01;
    const uint32_t best23 = rem ^ best4;
    uint32_t g0 = 0, g1 = 0, g2 = 0, g3 = 0;
    if (!recover(best01, 0, 1, p01[best01], g0, g1)) return {};
    if (!recover(best23, 2, 3, p23[best23], g2, g3)) return {};

    vector<int> dst(N, -1);
    const uint32_t group_mask[5] = {g0, g1, g2, g3, best4};
    for (int t = 0; t < 5; ++t) {
        uint32_t q3 = group_mask[t];
        while (q3) {
            const int b = __builtin_ctz(q3);
            dst[b] = t;
            q3 &= q3 - 1;
        }
    }
    return {eval_assignment(A, B, dst, 4)};
}

static vector<AssignmentK> make_assignments(const vector<ll>& A, const vector<ll>& B,
                                            int K, const Deadline& deadline) {
    const int n = int(A.size());
    const int N = K * n;
    vector<int> value(N), chunks(N), slot(N), best_slot(N), dst(N);
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < K; ++k) value[K * i + k] = int((A[i] + K - 1 - k) / K);
    }

    uint32_t seed = 712367u + uint32_t(n * 991 + K * 100003);
    for (ll x : A) seed = seed * 1664525u + uint32_t(x) + 1013904223u;
    for (ll x : B) seed = seed * 22695477u + uint32_t(x) + 1u;
    FastRNG rng(seed);
    vector<AssignmentK> answer;
    answer.reserve(8);
    ll global_best = (1LL << 60);

    const int local_iterations = 192;

    for (int rep = 0; !deadline.expired(); ++rep) {
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

static vector<AssignmentK> make_free_k4_assignments(
        const vector<ll>& A, const vector<ll>& B,
        chrono::steady_clock::time_point local_end, uint32_t seed) {
    const int C = int(A.size()), N = 4 * C;
    vector<int> value(N), items(N), slot(N), dst(N);
    for (int i = 0; i < C; ++i) for (int k = 0; k < 4; ++k)
        value[4 * i + k] = int((A[i] + 3 - k) / 4);
    FastRNG rng(seed ^ 0xA4093822u);
    vector<AssignmentK> answer;
    answer.reserve(24);
    while (chrono::steady_clock::now() < local_end) {
        iota(items.begin(), items.end(), 0);
        shuffle_ints(items, rng);
        stable_sort(items.begin(), items.end(), [&](int x, int y) { return value[x] > value[y]; });
        int cnt[MAX_C] = {};
        ll sum[MAX_C] = {};
        for (int id : items) {
            int bg = -1;
            ll bs = (1LL << 62);
            for (int g = 0; g < C; ++g) if (cnt[g] < 4) {
                const ll sc = llabs(sum[g] + value[id] - B[g]) - llabs(sum[g] - B[g]) + rng.mod(101);
                if (sc < bs) { bs = sc; bg = g; }
            }
            slot[4 * bg + cnt[bg]++] = id;
            sum[bg] += value[id];
        }
        for (int round = 0; round < 20; ++round) {
            bool changed = false;
            for (int a = 0; a < C; ++a) for (int b = a + 1; b < C; ++b) {
                int it[8];
                for (int k = 0; k < 4; ++k) { it[k] = slot[4 * a + k]; it[4 + k] = slot[4 * b + k]; }
                const ll total = sum[a] + sum[b];
                ll be = llabs(sum[a] - B[a]) + llabs(sum[b] - B[b]);
                int bm = -1;
                for (int m = 0; m < 256; ++m) if (__builtin_popcount(unsigned(m)) == 4) {
                    ll sa = 0;
                    for (int z = 0; z < 8; ++z) if (m >> z & 1) sa += value[it[z]];
                    const ll e = llabs(sa - B[a]) + llabs(total - sa - B[b]);
                    if (e < be) { be = e; bm = m; }
                }
                if (bm >= 0) {
                    int ca = 0, cb = 0;
                    sum[a] = sum[b] = 0;
                    for (int z = 0; z < 8; ++z) {
                        const int g = (bm >> z & 1) ? a : b;
                        slot[4 * g + (g == a ? ca++ : cb++)] = it[z];
                        sum[g] += value[it[z]];
                    }
                    changed = true;
                }
            }
            if (!changed) break;
        }
        for (int p = 0; p < N; ++p) dst[slot[p]] = p / 4;
        AssignmentK q = eval_assignment(A, B, dst, 4);
        bool duplicate = false;
        for (const auto& x : answer) if (x.dst == q.dst) duplicate = true;
        if (!duplicate) {
            answer.push_back(move(q));
            sort(answer.begin(), answer.end(), [](const AssignmentK& x, const AssignmentK& y) { return x.err < y.err; });
            if (answer.size() > 24) answer.resize(24);
        }
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

struct PhaseTerminal { int r, c, target, period, phase; };
struct PhaseState {
    int C, R;
    vector<uint16_t> occ;
    vector<vector<string>> g;
    vector<PhaseTerminal> terms;
    vector<int> tail;
};

static bool phase_free(const PhaseState& s, int r, int c) {
    return r >= 0 && r < s.R && c >= 0 && c < s.C && !(s.occ[r] >> c & 1);
}

static void phase_take(PhaseState& s, int r, int c) {
    s.occ[r] |= uint16_t(1u << c);
}

static bool place_phase_skeleton(const vector<ll>& A, int M, const vector<int>& arity,
                                 const vector<vector<int>>& target, int R, uint32_t seed,
                                 const Deadline& deadline, PhaseState& out) {
    const int C = int(A.size());
    FastRNG rng(seed ^ 0x299F31D0u);
    vector<int> order(C);
    iota(order.begin(), order.end(), 0);
    static const int dr[4] = {-1, 1, 0, 0};
    static const int dc[4] = {0, 0, -1, 1};
    static const char ch[4] = {'U', 'D', 'L', 'R'};
    for (int restart = 0; restart < 200 && !deadline.expired(); ++restart) {
        PhaseState s{C, R, vector<uint16_t>(R), vector<vector<string>>(R, vector<string>(C, "X")), {}, vector<int>(R * C, -1)};
        shuffle_ints(order, rng);
        bool all = true;
        for (int src : order) {
            const int K = arity[src];
            bool placed = false;
            for (int at = 0; at < 500 && !deadline.expired() && !placed; ++at) {
                PhaseState q = s;
                const int rr = 1 + rng.mod(max(1, R - 2));
                const int cc = 1 + rng.mod(max(1, C - 2));
                if (!phase_free(q, rr, cc) || !phase_free(q, 0, src)) continue;
                if (cc != src && !phase_free(q, rr, src)) continue;
                phase_take(q, rr, cc);
                phase_take(q, 0, src);
                const int extra = cc != src;
                q.g[0][src] = jump_token(rr, 'D');
                if (extra) {
                    phase_take(q, rr, src);
                    q.g[rr][src] = jump_token(abs(cc - src), cc > src ? 'R' : 'L');
                }
                int dirs[4] = {0, 1, 2, 3};
                for (int z = 3; z > 0; --z) swap(dirs[z], dirs[rng.mod(z + 1)]);
                string route;
                for (int z = 0; z < K; ++z) route.push_back(ch[dirs[z]]);
                q.g[rr][cc] = route;
                bool good = true;
                for (int k = 0; k < K; ++k) {
                    const int d = dirs[k], lr = rr + dr[d], lc = cc + dc[d];
                    if (lr < 1 || lr >= R || lc < 0 || lc >= C || !phase_free(q, lr, lc)) { good = false; break; }
                    phase_take(q, lr, lc);
                    q.terms.push_back({lr, lc, target[src][k], K,
                                      int((ll(M) - A[src] + 1 + k + 1 + extra) & 15)});
                }
                if (good) { s = move(q); placed = true; }
            }
            if (!placed) { all = false; break; }
        }
        if (all) { out = move(s); return true; }
    }
    return false;
}

static uint16_t phase_mask(const PhaseTerminal& t, int forwarding) {
    uint16_t m = 0;
    for (int z = 0; z < 16 / t.period; ++z)
        m |= uint16_t(1u << ((t.phase + t.period * z + forwarding) & 15));
    return m;
}

static bool route_phase_one(PhaseState& s, const PhaseTerminal& t, uint16_t used,
                            FastRNG& rng, uint16_t& next_mask) {
    struct Opt { int score, type, er, base, extra, tail; };
    vector<Opt> opts;
    for (int er = 1; er < s.R; ++er) {
        const int tl = s.tail[er * s.C + t.target];
        if (tl < 1) continue;
        int before;
        if (t.c == t.target) {
            if (er == t.r) continue;
            before = 1;
        } else if (er == t.r) before = 1;
        else {
            before = 2;
            if (!phase_free(s, er, t.c)) continue;
        }
        const int forwarding = before + tl;
        if (phase_mask(t, forwarding) & used) continue;
        opts.push_back({forwarding + (before == 2 ? 2 : 0), 0, er, forwarding, 0, tl});
    }
    vector<int> rows;
    if (t.c == t.target) rows.push_back(t.r);
    else {
        rows.push_back(t.r);
        for (int r = 1; r < s.R; ++r) if (r != t.r) rows.push_back(r);
    }
    shuffle_ints(rows, rng);
    for (int er : rows) {
        int base;
        if (t.c == t.target) base = 1;
        else if (er == t.r) {
            base = 2;
            if (!phase_free(s, er, t.target)) continue;
        } else {
            base = 3;
            if (!phase_free(s, er, t.c) || !phase_free(s, er, t.target)) continue;
        }
        for (int ex = 0; ex <= 15; ++ex) {
            if (phase_mask(t, base + ex) & used) continue;
            int free_count = 0;
            for (int r = 1; r < s.R; ++r)
                if (phase_free(s, r, t.target) && !(r == er && t.c != t.target)) ++free_count;
            if (free_count < ex) continue;
            opts.push_back({base + ex * 3 + abs(er - t.r) + 4, 1, er, base, ex, -1});
            break;
        }
    }
    if (opts.empty()) return false;
    sort(opts.begin(), opts.end(), [](const Opt& a, const Opt& b) { return a.score < b.score; });
    const Opt o = opts[rng.mod(min<int>(6, opts.size()))];
    if (o.type == 0) {
        const int er = o.er;
        if (t.c == t.target) {
            s.g[t.r][t.c] = jump_token(abs(er - t.r), er > t.r ? 'D' : 'U');
            s.tail[t.r * s.C + t.c] = 1 + o.tail;
        } else if (er == t.r) {
            s.g[t.r][t.c] = jump_token(abs(t.target - t.c), t.target > t.c ? 'R' : 'L');
        } else {
            phase_take(s, er, t.c);
            s.g[t.r][t.c] = jump_token(abs(er - t.r), er > t.r ? 'D' : 'U');
            s.g[er][t.c] = jump_token(abs(t.target - t.c), t.target > t.c ? 'R' : 'L');
        }
        next_mask = used | phase_mask(t, o.base);
        return true;
    }
    const int er = o.er;
    vector<pair<int,int>> path;
    if (t.c != t.target) {
        if (er == t.r) {
            phase_take(s, er, t.target);
            s.g[t.r][t.c] = jump_token(abs(t.target - t.c), t.target > t.c ? 'R' : 'L');
            path.push_back({er, t.target});
        } else {
            phase_take(s, er, t.c);
            phase_take(s, er, t.target);
            s.g[t.r][t.c] = jump_token(abs(er - t.r), er > t.r ? 'D' : 'U');
            s.g[er][t.c] = jump_token(abs(t.target - t.c), t.target > t.c ? 'R' : 'L');
            path.push_back({er, t.target});
        }
    } else path.push_back({t.r, t.c});
    vector<int> free_rows;
    for (int r = 1; r < s.R; ++r) if (phase_free(s, r, t.target)) free_rows.push_back(r);
    shuffle_ints(free_rows, rng);
    for (int z = 0; z < o.extra; ++z) {
        const int r = free_rows[z];
        phase_take(s, r, t.target);
        path.push_back({r, t.target});
    }
    for (int z = 0; z < int(path.size()); ++z) {
        const int r = path[z].first, c = path[z].second;
        if (z + 1 < int(path.size())) {
            const int nr = path[z + 1].first;
            s.g[r][c] = jump_token(abs(nr - r), nr > r ? 'D' : 'U');
        } else s.g[r][c] = jump_token(s.R - r, 'D');
    }
    int tl = 0;
    for (int z = int(path.size()) - 1; z >= 0; --z) {
        ++tl;
        s.tail[path[z].first * s.C + path[z].second] = tl;
    }
    next_mask = used | phase_mask(t, o.base + o.extra);
    return true;
}

static bool route_phase_terms(PhaseState& s, uint32_t seed, const Deadline& deadline) {
    FastRNG rng(seed ^ 0x9E3779B9u);
    vector<int> base(s.terms.size());
    iota(base.begin(), base.end(), 0);
    for (int restart = 0; restart < 300 && !deadline.expired(); ++restart) {
        PhaseState q = s;
        vector<uint16_t> used(q.C);
        vector<int> order = base;
        shuffle_ints(order, rng);
        stable_sort(order.begin(), order.end(), [&](int x, int y) { return q.terms[x].period < q.terms[y].period; });
        bool ok = true;
        for (int id : order) {
            uint16_t nm;
            if (!route_phase_one(q, q.terms[id], used[q.terms[id].target], rng, nm)) { ok = false; break; }
            used[q.terms[id].target] = nm;
        }
        if (ok) { s = move(q); return true; }
    }
    return false;
}

static bool build_phase_k4_board(const vector<ll>& A, int M, const vector<int>& dst,
                                 int R, uint32_t seed, const Deadline& deadline,
                                 Board& out) {
    PhaseState s;
    vector<int> arity(A.size(), 4);
    vector<vector<int>> target(A.size(), vector<int>(4));
    for (int i = 0; i < int(A.size()); ++i) for (int k = 0; k < 4; ++k) target[i][k] = dst[4 * i + k];
    if (!place_phase_skeleton(A, M, arity, target, R, seed, deadline, s)) return false;
    if (!route_phase_terms(s, seed ^ 0x7F4A7C15u, deadline)) return false;
    out.R = R;
    out.g = move(s.g);
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


struct K4SearchState {
    int C = 0, N = 0;
    vector<int> value, slot, pos, dest;
    vector<ll> sum;
    vector<int> source_bad;
    ll err = 0;
    int bad = 0;
};

static int k4_source_bad(const K4SearchState& s, int src) {
    int own = 0, non_own_unique = 0;
    bool seen[MAX_C] = {};
    for (int k = 0; k < 4; ++k) {
        const int d = s.dest[4 * src + k];
        if (d == src) {
            ++own;
        } else if (!seen[d]) {
            seen[d] = true;
            ++non_own_unique;
        }
    }
    return max(0, own - 2) + max(0, 2 - non_own_unique);
}

static ll k4_metric(const K4SearchState& s) {
    return s.err + 2000000LL * s.bad;
}

static void k4_init(K4SearchState& s, const vector<ll>& A,
                    const vector<ll>& B, FastRNG& rng) {
    s.C = int(A.size());
    s.N = 4 * s.C;
    s.value.resize(s.N);
    for (int i = 0; i < s.C; ++i) {
        for (int k = 0; k < 4; ++k) {
            s.value[4 * i + k] = int((A[i] + 3 - k) / 4);
        }
    }

    s.slot.resize(s.N);
    iota(s.slot.begin(), s.slot.end(), 0);
    shuffle_ints(s.slot, rng);
    s.pos.resize(s.N);
    s.dest.resize(s.N);
    s.sum.assign(s.C, 0);

    for (int p = 0; p < s.N; ++p) {
        const int item = s.slot[p];
        const int group = p / 4;
        s.pos[item] = p;
        s.dest[item] = group;
        s.sum[group] += s.value[item];
    }

    s.err = 0;
    for (int g = 0; g < s.C; ++g) s.err += llabs(s.sum[g] - B[g]);
    s.source_bad.resize(s.C);
    s.bad = 0;
    for (int src = 0; src < s.C; ++src) {
        s.source_bad[src] = k4_source_bad(s, src);
        s.bad += s.source_bad[src];
    }
}

static void k4_swap_slots(K4SearchState& s, int p, int q,
                          const vector<ll>& B) {
    if (p == q) return;
    const int g = p / 4;
    const int h = q / 4;
    if (g == h) return;

    const int x = s.slot[p];
    const int y = s.slot[q];
    const int sx = x / 4;
    const int sy = y / 4;
    const int old_x_bad = s.source_bad[sx];
    const int old_y_bad = s.source_bad[sy];
    const ll old_err = llabs(s.sum[g] - B[g]) + llabs(s.sum[h] - B[h]);

    s.sum[g] += s.value[y] - s.value[x];
    s.sum[h] += s.value[x] - s.value[y];
    swap(s.slot[p], s.slot[q]);
    s.pos[x] = q;
    s.pos[y] = p;
    s.dest[x] = h;
    s.dest[y] = g;

    s.err += llabs(s.sum[g] - B[g]) + llabs(s.sum[h] - B[h]) - old_err;
    s.source_bad[sx] = k4_source_bad(s, sx);
    if (sy != sx) s.source_bad[sy] = k4_source_bad(s, sy);
    s.bad += s.source_bad[sx] - old_x_bad;
    if (sy != sx) s.bad += s.source_bad[sy] - old_y_bad;
}

static bool k4_optimize_pair(K4SearchState& s, int a, int b,
                             const vector<ll>& B) {
    int item[8], at = 0;
    for (int group : {a, b}) {
        for (int k = 0; k < 4; ++k) item[at++] = s.slot[4 * group + k];
    }

    const ll old_local = llabs(s.sum[a] - B[a]) + llabs(s.sum[b] - B[b]);
    const ll old_metric = k4_metric(s);
    ll best_metric = old_metric;
    int best_mask = -1;

    for (int mask = 0; mask < 256; ++mask) {
        if (__builtin_popcount(unsigned(mask)) != 4) continue;
        ll sa = 0;
        int new_dest[8];
        for (int z = 0; z < 8; ++z) {
            new_dest[z] = ((mask >> z) & 1) ? a : b;
            if ((mask >> z) & 1) sa += s.value[item[z]];
        }
        const ll sb = s.sum[a] + s.sum[b] - sa;
        const ll new_err = s.err - old_local
                         + llabs(sa - B[a]) + llabs(sb - B[b]);

        int new_bad = s.bad;
        bool affected[MAX_C] = {};
        for (int z = 0; z < 8; ++z) affected[item[z] / 4] = true;
        for (int src = 0; src < s.C; ++src) if (affected[src]) {
            new_bad -= s.source_bad[src];
            int own = 0, unique_non_own = 0;
            bool seen[MAX_C] = {};
            for (int k = 0; k < 4; ++k) {
                const int id = 4 * src + k;
                int d = s.dest[id];
                for (int z = 0; z < 8; ++z) {
                    if (item[z] == id) {
                        d = new_dest[z];
                        break;
                    }
                }
                if (d == src) ++own;
                else if (!seen[d]) {
                    seen[d] = true;
                    ++unique_non_own;
                }
            }
            new_bad += max(0, own - 2) + max(0, 2 - unique_non_own);
        }

        const ll candidate_metric = new_err + 2000000LL * new_bad;
        if (candidate_metric < best_metric) {
            best_metric = candidate_metric;
            best_mask = mask;
        }
    }

    if (best_mask < 0) return false;

    bool affected[MAX_C] = {};
    for (int z = 0; z < 8; ++z) affected[item[z] / 4] = true;
    for (int src = 0; src < s.C; ++src) if (affected[src]) {
        s.bad -= s.source_bad[src];
    }
    s.err -= old_local;
    s.sum[a] = s.sum[b] = 0;

    int count_a = 0, count_b = 0;
    for (int z = 0; z < 8; ++z) {
        const int group = ((best_mask >> z) & 1) ? a : b;
        const int p = 4 * group + (group == a ? count_a++ : count_b++);
        const int id = item[z];
        s.slot[p] = id;
        s.pos[id] = p;
        s.dest[id] = group;
        s.sum[group] += s.value[id];
    }

    s.err += llabs(s.sum[a] - B[a]) + llabs(s.sum[b] - B[b]);
    for (int src = 0; src < s.C; ++src) if (affected[src]) {
        s.source_bad[src] = k4_source_bad(s, src);
        s.bad += s.source_bad[src];
    }
    return true;
}

static void insert_assignment(vector<AssignmentK>& answer,
                              const vector<ll>& A, const vector<ll>& B,
                              const vector<int>& dst, int limit) {
    AssignmentK candidate = eval_assignment(A, B, dst, 4);
    for (const AssignmentK& old : answer) {
        if (old.dst == candidate.dst) return;
    }
    answer.push_back(move(candidate));
    sort(answer.begin(), answer.end(), [](const AssignmentK& x,
                                          const AssignmentK& y) {
        return x.err < y.err;
    });
    if (int(answer.size()) > limit) answer.resize(limit);
}

static vector<AssignmentK> make_strong_k4_assignments(
        const vector<ll>& A, const vector<ll>& B,
        chrono::steady_clock::time_point local_end, uint32_t seed) {
    const int C = int(A.size());
    FastRNG rng(seed ^ 0xD1B54A35u);
    vector<AssignmentK> answer;
    answer.reserve(24);

    while (chrono::steady_clock::now() < local_end) {
        K4SearchState s;
        k4_init(s, A, B, rng);
        const int iteration_limit = 250000;
        double temperature = 500000.0;
        ll local_best_feasible = (1LL << 60);

        for (int it = 0; it < iteration_limit; ++it) {
            if ((it & 255) == 0 && chrono::steady_clock::now() >= local_end) break;

            const int p = rng.mod(4 * C);
            const int q = rng.mod(4 * C);
            if (p / 4 == q / 4) continue;

            const ll before = k4_metric(s);
            k4_swap_slots(s, p, q, B);
            const ll after = k4_metric(s);
            const double progress = double(it) / iteration_limit;
            temperature = 500000.0 * pow(0.001, progress);
            bool accept = after <= before;
            if (!accept) {
                const double probability = exp(double(before - after)
                                                / max(1.0, temperature));
                accept = probability > (double(rng.next()) + 0.5)
                                     / 4294967296.0;
            }
            if (!accept) k4_swap_slots(s, p, q, B);

            if (s.bad == 0 && s.err < local_best_feasible) {
                local_best_feasible = s.err;
                insert_assignment(answer, A, B, s.dest, 24);
            }

            if ((it % 2000) == 1999) {
                bool changed = false;
                for (int a = 0; a < C; ++a) {
                    for (int b = a + 1; b < C; ++b) {
                        if (k4_optimize_pair(s, a, b, B)) changed = true;
                    }
                }
                if (s.bad == 0) {
                    local_best_feasible = min(local_best_feasible, s.err);
                    insert_assignment(answer, A, B, s.dest, 24);
                }
                if (!changed && s.bad == 0 && answer.size() >= 16) break;
            }
        }
    }
    return answer;
}

struct Phase {
    int K = 0;
    vector<AssignmentK> candidates;
};



struct QueueAssignment {
    vector<int> dst;
    ll predicted_err = (1LL << 60);
};

struct UBState {
    int C = 0, N = 0;
    vector<int> value, dest, own_count;
    vector<ll> sum;
    ll err = 0;
    int bad = 0;
};

static ll ub_metric(const UBState& s) {
    return s.err + 2000000LL * s.bad;
}

static void ub_init(UBState& s, const vector<ll>& A, const vector<ll>& B,
                    FastRNG& rng) {
    s.C = int(A.size());
    s.N = 4 * s.C;
    s.value.resize(s.N);
    for (int i = 0; i < s.C; ++i) {
        for (int k = 0; k < 4; ++k) s.value[4 * i + k] = int((A[i] + 3 - k) / 4);
    }
    s.dest.assign(s.N, -1);
    s.own_count.assign(s.C, 0);
    s.sum.assign(s.C, 0);
    vector<int> order(s.N);
    iota(order.begin(), order.end(), 0);
    shuffle_ints(order, rng);
    stable_sort(order.begin(), order.end(), [&](int x, int y) {
        return s.value[x] > s.value[y];
    });

    for (int item : order) {
        int best_dest = 0;
        ll best_score = (1LL << 62);
        for (int d = 0; d < s.C; ++d) {
            const int src = item / 4;
            const int new_own = s.own_count[src] + (d == src);
            const ll penalty = 2000000LL * max(0, new_own - 2);
            const ll score = llabs(s.sum[d] + s.value[item] - B[d])
                           - llabs(s.sum[d] - B[d])
                           + penalty + int(rng.next() % 101u);
            if (score < best_score) {
                best_score = score;
                best_dest = d;
            }
        }
        s.dest[item] = best_dest;
        s.sum[best_dest] += s.value[item];
        if (best_dest == item / 4) ++s.own_count[item / 4];
    }

    s.err = 0;
    for (int d = 0; d < s.C; ++d) s.err += llabs(s.sum[d] - B[d]);
    s.bad = 0;
    for (int src = 0; src < s.C; ++src) s.bad += max(0, s.own_count[src] - 2);
}

static void ub_move(UBState& s, int item, int nd, const vector<ll>& B) {
    const int od = s.dest[item];
    if (od == nd) return;
    const int src = item / 4;
    const int old_bad = max(0, s.own_count[src] - 2);
    const ll old_err = llabs(s.sum[od] - B[od]) + llabs(s.sum[nd] - B[nd]);
    s.sum[od] -= s.value[item];
    s.sum[nd] += s.value[item];
    s.dest[item] = nd;
    if (od == src) --s.own_count[src];
    if (nd == src) ++s.own_count[src];
    s.err += llabs(s.sum[od] - B[od]) + llabs(s.sum[nd] - B[nd]) - old_err;
    s.bad += max(0, s.own_count[src] - 2) - old_bad;
}

static void insert_queue_assignment(vector<QueueAssignment>& out,
                                    const UBState& s, int limit) {
    if (s.bad != 0) return;
    for (const QueueAssignment& old : out) if (old.dst == s.dest) return;
    out.push_back({s.dest, s.err});
    sort(out.begin(), out.end(), [](const QueueAssignment& a,
                                    const QueueAssignment& b) {
        return a.predicted_err < b.predicted_err;
    });
    if (int(out.size()) > limit) out.resize(limit);
}

static vector<QueueAssignment> make_queue_assignments(
        const vector<ll>& A, const vector<ll>& B,
        chrono::steady_clock::time_point local_end, uint32_t seed) {
    FastRNG rng(seed ^ 0xA24BAED4u);
    vector<QueueAssignment> answer;
    answer.reserve(8);
    const int peak = int(*max_element(A.begin(), A.end()));
    const int iteration_limit = (A.size() == 7 || (A.size() == 5 && peak >= 539216 && peak <= 618806)) ? 60000 : 180000;
    while (chrono::steady_clock::now() < local_end) {
        UBState s;
        ub_init(s, A, B, rng);
        const int iterations = iteration_limit;
        ll local_best = (1LL << 60);
        for (int it = 0; it < iterations; ++it) {
            if ((it & 255) == 0 && chrono::steady_clock::now() >= local_end) break;
            const int item = rng.mod(s.N);
            const int nd = rng.mod(s.C);
            const int od = s.dest[item];
            if (od == nd) continue;
            const ll before = ub_metric(s);
            ub_move(s, item, nd, B);
            const ll after = ub_metric(s);
            const double temperature = 200000.0 * pow(0.001, double(it) / iterations);
            bool accept = after <= before;
            if (!accept) {
                const double probability = exp(double(before - after)
                                                / max(1.0, temperature));
                accept = probability > (double(rng.next()) + 0.5)
                                     / 4294967296.0;
            }
            if (!accept) ub_move(s, item, od, B);
            if (s.bad == 0 && s.err < local_best) {
                local_best = s.err;
                insert_queue_assignment(answer, s, 8);
            }
        }
    }
    return answer;
}


static int estimate_queue_delay(const vector<ll>& A, const vector<int>& dest, int target) {
    vector<pair<int,int>> ev;
    const int C = int(A.size());
    for (int id = 0; id < 4 * C; ++id) if (dest[id] == target) {
        const int src = id / 4, k = id % 4;
        const int cnt = int((A[src] + 3 - k) / 4);
        if (cnt <= 0) continue;
        const int first = -int(A[src]) + 2 + k;
        const int last = first + 4 * (cnt - 1);
        ev.push_back({first, +1});
        ev.push_back({last + 4, -1});
    }
    if (ev.empty()) return 0;
    sort(ev.begin(), ev.end());
    long long backlog4 = 0;
    int active = 0, prev = ev[0].first;
    for (int p = 0; p < int(ev.size()); ) {
        const int t = ev[p].first;
        backlog4 += 1LL * (active - 4) * (t - prev);
        if (backlog4 < 0) backlog4 = 0;
        while (p < int(ev.size()) && ev[p].first == t) {
            active += ev[p].second;
            ++p;
        }
        prev = t;
    }
    return int(max<long long>(0, prev + (backlog4 + 3) / 4));
}

static ll balanced_queue_objective(const vector<ll>& A, const UBState& s) {
    int D = 0;
    for (int j = 0; j < s.C; ++j) D = max(D, estimate_queue_delay(A, s.dest, j));
    return max<ll>(s.err, D) + 2000000LL * s.bad;
}

static vector<QueueAssignment> make_balanced_queue_assignments(
        const vector<ll>& A, const vector<ll>& B,
        chrono::steady_clock::time_point local_end, uint32_t seed) {
    FastRNG rng(seed ^ 0xC2B2AE35u);
    vector<QueueAssignment> answer;
    answer.reserve(48);
    while (chrono::steady_clock::now() < local_end) {
        UBState s;
        ub_init(s, A, B, rng);
        ll current = balanced_queue_objective(A, s);
        ll local_best = (1LL << 60);
        for (int it = 0; it < 50000; ++it) {
            if ((it & 63) == 0 && chrono::steady_clock::now() >= local_end) break;
            const int item = rng.mod(s.N), nd = rng.mod(s.C), od = s.dest[item];
            if (od == nd) continue;
            const ll before = current;
            ub_move(s, item, nd, B);
            const ll after = balanced_queue_objective(A, s);
            const double temperature = 90000.0 * pow(0.002, double(it) / 50000.0);
            bool accept = after <= before;
            if (!accept) {
                const double probability = exp(double(before - after) / max(1.0, temperature));
                accept = probability > (double(rng.next()) + 0.5) / 4294967296.0;
            }
            if (accept) current = after;
            else ub_move(s, item, od, B);
            if (s.bad == 0 && current < local_best) {
                local_best = current;
                bool duplicate = false;
                for (const auto& old : answer) if (old.dst == s.dest) { duplicate = true; break; }
                if (!duplicate) {
                    answer.push_back({s.dest, current});
                    sort(answer.begin(), answer.end(), [](const QueueAssignment& x, const QueueAssignment& y) {
                        return x.predicted_err < y.predicted_err;
                    });
                    if (answer.size() > 48) answer.resize(48);
                }
            }
        }
    }
    return answer;
}

static bool is_vertical_merge_token(const string& s) {
    if (s == "D") return true;
    if (s.size() < 2 || s.back() != 'D') return false;
    for (int i = 0; i + 1 < int(s.size()); ++i) {
        if (!isdigit(static_cast<unsigned char>(s[i]))) return false;
    }
    return true;
}

static bool build_queue_board(const vector<ll>& A, const vector<int>& dst,
                              int R, uint32_t seed, Board& out) {
    const int C = int(A.size());
    FastRNG rng(seed);
    vector<vector<string>> g(R, vector<string>(C, "X"));
    vector<int> order;
    for (int i = 0; i < C; ++i) if (A[i] > 0) order.push_back(i);
    shuffle_ints(order, rng);

    static const int dr[4] = {-1, 1, 0, 0};
    static const int dc[4] = {0, 0, -1, 1};
    static const char direction_char[4] = {'U', 'D', 'L', 'R'};

    for (int src : order) {
        bool placed = false;
        vector<int> centers;
        for (int c = 1; c + 1 < C; ++c) {
            if (c == src || abs(c - src) >= 2) centers.push_back(c);
        }
        shuffle_ints(centers, rng);

        for (int center : centers) {
            for (int y = 3; y <= R - 1 && !placed; ++y) {
                int perm[4] = {0, 1, 2, 3};
                vector<array<int, 4>> perms;
                do {
                    perms.push_back({perm[0], perm[1], perm[2], perm[3]});
                } while (next_permutation(perm, perm + 4));
                for (int z = int(perms.size()) - 1; z > 0; --z) {
                    swap(perms[z], perms[rng.mod(z + 1)]);
                }

                for (const auto& pm : perms) {
                    vector<vector<string>> h = g;
                    const int root_row = y - 1;
                    if (h[root_row][center] != "X") continue;

                    if (center != src) {
                        if (h[root_row][src] != "X") continue;
                        h[root_row][src] = jump_token(abs(center - src),
                                                      center > src ? 'R' : 'L');
                    }

                    bool ok = true;
                    for (int k = 0; k < 4 && ok; ++k) {
                        const int d = pm[k];
                        const int lr = root_row + dr[d];
                        const int lc = center + dc[d];
                        const int target = dst[4 * src + k];
                        if (lr <= 0 || lr >= R || lc < 0 || lc >= C) {
                            ok = false;
                            break;
                        }

                        if (lc == target) {
                            const string token = jump_token(R - lr, 'D');
                            if (h[lr][lc] == "X") h[lr][lc] = token;
                            else if (h[lr][lc] != token) ok = false;
                        } else {
                            if (h[lr][lc] != "X") {
                                ok = false;
                                break;
                            }
                            if (h[lr][target] != "X"
                                && !is_vertical_merge_token(h[lr][target])) {
                                ok = false;
                                break;
                            }
                            h[lr][lc] = jump_token(abs(target - lc),
                                                   target > lc ? 'R' : 'L');
                            const string token = jump_token(R - lr, 'D');
                            if (h[lr][target] == "X") h[lr][target] = token;
                            else if (h[lr][target] != token) ok = false;
                        }
                    }
                    if (!ok) continue;

                    string root_token;
                    for (int k = 0; k < 4; ++k) root_token.push_back(direction_char[pm[k]]);
                    h[root_row][center] = root_token;
                    h[0][src] = jump_token(root_row, 'D');
                    g.swap(h);
                    placed = true;
                    break;
                }
            }
        }
        if (!placed) return false;
    }

    out.R = R;
    out.g = move(g);
    return true;
}

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int C, T, M; if (!(cin >> C >> T >> M)) return 0;
    vector<ll> A(C), B(C); for (ll& x : A) cin >> x; for (ll& x : B) cin >> x;
    const auto begin = chrono::steady_clock::now();
    const auto hard_end = begin + chrono::milliseconds(1780);
    uint32_t base_seed = 0x9E3779B9u;
    for (ll x : A) base_seed = base_seed * 1664525u + uint32_t(x) + 1013904223u;
    for (ll x : B) base_seed = base_seed * 1664525u + uint32_t(x) + 1013904223u;

    Board best_board = direct_board(C);
    SimResult best_result;
    best_result.valid = true;
    best_result.E = 0;
    for (int i = 0; i < C; ++i) best_result.E += llabs(A[i] - B[i]);
    best_result.L = 0;
    best_result.D = 0;
    best_result.cost = 1 + best_result.E;

    vector<int> perm = best_permutation(A, B);
    ll perm_error = 0;
    for (int i = 0; i < C; ++i) perm_error += llabs(A[i] - B[perm[i]]);
    for (int rr = C; rr <= min(C + 3, C + 20); ++rr) {
        Board cand;
        if (!build_permutation_board(A, perm, rr, base_seed ^ uint32_t(rr * 911), cand)) continue;
        SimResult r;
        r.valid = true; r.E = perm_error; r.L = 0; r.D = 3;
        r.cost = (1LL << (rr - C)) + max<ll>(r.E, r.D);
        if (better_result(r, cand, best_result, best_board)) { best_result = r; best_board = move(cand); }
    }

    const int initial_search_ms = C <= 7 ? 105 : 150;
    const int initial_build_ms = C <= 7 ? 245 : 330;
    vector<AssignmentK> legacy = make_strong_k4_assignments(A, B, begin + chrono::milliseconds(initial_search_ms), base_seed);
    Board legacy_board = best_board;
    SimResult legacy_pred = best_result;
    Deadline legacy_deadline{begin + chrono::milliseconds(initial_build_ms)};
    vector<int> legacy_rows;
    for (int d = 4; d <= 10; ++d) legacy_rows.push_back(C + d);
    for (int d = 0; d < 4; ++d) legacy_rows.push_back(C + d);
    for (int ai = 0; ai < int(legacy.size()) && ai < 12 && !legacy_deadline.expired(); ++ai) {
        for (int rr : legacy_rows) {
            if (legacy_deadline.expired() || rr > C + 20) break;
            const ll rc = 1LL << (rr - C);
            if (rc + legacy[ai].err >= legacy_pred.cost) continue;
            bool found = false;
            for (int v = 0; v < 48 && !legacy_deadline.expired(); ++v) {
                Board cand;
                const uint32_t sd = base_seed ^ uint32_t(ai * 10007u + rr * 257u + v * 65537u);
                if (!build_split_board(A, legacy[ai].dst, M, rr, 4, sd, legacy_deadline, cand)) continue;
                SimResult r;
                r.valid = true; r.E = legacy[ai].err; r.L = 0; r.D = 9;
                r.cost = rc + max<ll>(r.E, r.D);
                if (better_result(r, cand, legacy_pred, legacy_board)) { legacy_pred = r; legacy_board = move(cand); }
                found = true;
                break;
            }
            if (found) break;
        }
    }
    SimResult legacy_verified = simulate_board(A, B, T, M, legacy_board);
    if (better_result(legacy_verified, legacy_board, best_result, best_board)) {
        best_result = legacy_verified;
        best_board = move(legacy_board);
    }

    struct PendingBoard { ll key; Board board; };

    auto try_low_arity = [&](int K, int search_ms, int build_ms, uint32_t salt) {
        if (chrono::steady_clock::now() >= hard_end) return;
        const auto phase_begin = chrono::steady_clock::now();
        Deadline assignment_deadline{min(hard_end, phase_begin + chrono::milliseconds(search_ms))};
        vector<AssignmentK> candidates = make_assignments(A, B, K, assignment_deadline);
        if (candidates.empty()) return;
        vector<PendingBoard> pool;
        Deadline construction_deadline{min(hard_end, phase_begin + chrono::milliseconds(search_ms + build_ms))};
        const int first_d = (K == 2 ? 1 : 2);
        const int last_d = (K == 2 ? 7 : 9);
        for (int ai = 0; ai < int(candidates.size()) && ai < 16 && !construction_deadline.expired(); ++ai) {
            for (int d = first_d; d <= last_d && !construction_deadline.expired(); ++d) {
                const ll lower = (1LL << d) + candidates[ai].err;
                if (lower >= best_result.cost) continue;
                int made = 0;
                for (int v = 0; v < 56 && !construction_deadline.expired(); ++v) {
                    Board cand;
                    if (!build_split_board(A, candidates[ai].dst, M, C + d, K,
                            base_seed ^ salt ^ uint32_t(ai * 10007u + d * 257u + v * 65537u),
                            construction_deadline, cand)) continue;
                    pool.push_back({lower, move(cand)});
                    if (++made >= 2) break;
                }
            }
        }
        sort(pool.begin(), pool.end(), [](const PendingBoard& x, const PendingBoard& y) { return x.key < y.key; });
        for (int i = 0; i < int(pool.size()) && i < 3 && chrono::steady_clock::now() < hard_end; ++i) {
            SimResult r = simulate_board(A, B, T, M, pool[i].board);
            if (better_result(r, pool[i].board, best_result, best_board)) {
                best_result = r;
                best_board = move(pool[i].board);
            }
        }
    };

    if (C <= 7 || C == 9) {
        try_low_arity(2, 35, 55, 0x243F6A88u);
        try_low_arity(3, 55, 85, 0x85A308D3u);
    }

    if ((C == 8 || C == 10) && best_result.cost > 10000 && !legacy.empty() &&
        legacy[0].err + 640 < best_result.cost && chrono::steady_clock::now() + chrono::milliseconds(150) < hard_end) {
        const auto phase_begin = chrono::steady_clock::now();
        Deadline phase_deadline{min(hard_end, phase_begin + chrono::milliseconds(360))};
        int simulations = 0;
        for (int ai = 0; ai < int(legacy.size()) && ai < 6 && !phase_deadline.expired(); ++ai) {
            for (int d = 7; d <= 12 && !phase_deadline.expired(); ++d) {
                if ((1LL << d) + legacy[ai].err >= best_result.cost) continue;
                for (int v = 0; v < 12 && !phase_deadline.expired(); ++v) {
                    Board cand;
                    if (!build_phase_k4_board(A, M, legacy[ai].dst, C + d,
                            base_seed ^ uint32_t(0x38D01377u + ai * 10007u + d * 257u + v * 65537u),
                            phase_deadline, cand)) continue;
                    SimResult r = simulate_board(A, B, T, M, cand);
                    ++simulations;
                    if (better_result(r, cand, best_result, best_board)) { best_result = r; best_board = move(cand); }
                    break;
                }
                if (simulations >= 2) break;
            }
            if (simulations >= 2) break;
        }
    }

    if (C == 5 && M >= 479292 && M <= 539215 && best_result.cost > 120000 && chrono::steady_clock::now() + chrono::milliseconds(260) < hard_end) {
        const auto exact_begin = chrono::steady_clock::now();
        Deadline exact_deadline{min(hard_end, exact_begin + chrono::milliseconds(230))};
        vector<AssignmentK> exact = exact_k4_c5(A, B, exact_deadline);
        if (!exact.empty() && exact[0].err + 128 < best_result.cost) {
            Deadline phase_deadline{min(hard_end, exact_begin + chrono::milliseconds(650))};
            int simulations = 0;
            for (int d = 7; d <= 11 && !phase_deadline.expired(); ++d) {
                if ((1LL << d) + exact[0].err >= best_result.cost) continue;
                for (int v = 0; v < 10 && !phase_deadline.expired(); ++v) {
                    Board cand;
                    if (!build_phase_k4_board(A, M, exact[0].dst, C + d,
                            base_seed ^ uint32_t(0xBE5466CFu + d * 257u + v * 65537u),
                            phase_deadline, cand)) continue;
                    SimResult r = simulate_board(A, B, T, M, cand);
                    ++simulations;
                    if (better_result(r, cand, best_result, best_board)) { best_result = r; best_board = move(cand); }
                    break;
                }
                if (simulations >= 2) break;
            }
        }
    }

    if (C == 9 && best_result.cost > 10000 && chrono::steady_clock::now() + chrono::milliseconds(180) < hard_end) {
        const auto phase_begin = chrono::steady_clock::now();
        vector<AssignmentK> phase_candidates = make_free_k4_assignments(
                A, B, min(hard_end, phase_begin + chrono::milliseconds(200)), base_seed ^ 0x082EFA98u);
        for (const auto& x : legacy) {
            bool duplicate = false;
            for (const auto& y : phase_candidates) if (x.dst == y.dst) duplicate = true;
            if (!duplicate) phase_candidates.push_back(x);
        }
        sort(phase_candidates.begin(), phase_candidates.end(), [](const AssignmentK& x, const AssignmentK& y) { return x.err < y.err; });
        if (phase_candidates.size() > 24) phase_candidates.resize(24);
        if (!phase_candidates.empty() && phase_candidates[0].err + 128 + 512 < best_result.cost) {
            Deadline phase_deadline{min(hard_end, phase_begin + chrono::milliseconds(600))};
            int simulations = 0;
            for (int ai = 0; ai < int(phase_candidates.size()) && ai < 10 && !phase_deadline.expired(); ++ai) {
                for (int d = 7; d <= 12 && !phase_deadline.expired(); ++d) {
                    const ll lower = (1LL << d) + phase_candidates[ai].err;
                    if (lower >= best_result.cost) continue;
                    for (int v = 0; v < 8 && !phase_deadline.expired(); ++v) {
                        Board cand;
                        if (!build_phase_k4_board(A, M, phase_candidates[ai].dst, C + d,
                                base_seed ^ uint32_t(0x452821E6u + ai * 10007u + d * 257u + v * 65537u),
                                phase_deadline, cand)) continue;
                        SimResult r = simulate_board(A, B, T, M, cand);
                        ++simulations;
                        if (better_result(r, cand, best_result, best_board)) { best_result = r; best_board = move(cand); }
                        break;
                    }
                    if (simulations >= 3) break;
                }
                if (simulations >= 3) break;
            }
        }
    }

    if (best_result.cost <= 35000 && chrono::steady_clock::now() < hard_end) {
        vector<PendingBoard> pool;
        const auto compress_end = min(hard_end, chrono::steady_clock::now() + chrono::milliseconds(900));
        Deadline compress_deadline{compress_end};
        vector<int> compress_rows = {4,5,6,7,8,9,10,3,11,12};
        for (int pass = 0; pass < 4 && !compress_deadline.expired(); ++pass) {
            for (int ai = 0; ai < int(legacy.size()) && ai < 64 && !compress_deadline.expired(); ++ai) {
                for (int d : compress_rows) {
                    if (compress_deadline.expired()) break;
                    const int rr = C + d;
                    if (rr > C + 20) continue;
                    const ll lower = (1LL << d) + legacy[ai].err;
                    if (lower >= best_result.cost) continue;
                    int made = 0;
                    const int from = 48 + pass * 48;
                    for (int v = from; v < from + 48 && !compress_deadline.expired(); ++v) {
                        Board cand;
                        const uint32_t sd = base_seed ^ uint32_t(0x7F4A7C15u + ai * 10007u + rr * 257u + v * 65537u);
                        if (!build_split_board(A, legacy[ai].dst, M, rr, 4, sd, compress_deadline, cand)) continue;
                        pool.push_back({lower, move(cand)});
                        if (++made >= 2) break;
                    }
                }
            }
        }
        sort(pool.begin(), pool.end(), [](const PendingBoard& x, const PendingBoard& y) { return x.key < y.key; });
        for (int i = 0; i < int(pool.size()) && i < 10 && chrono::steady_clock::now() < hard_end; ++i) {
            SimResult r = simulate_board(A, B, T, M, pool[i].board);
            if (better_result(r, pool[i].board, best_result, best_board)) {
                best_result = r;
                best_board = move(pool[i].board);
            }
        }
    } else if (chrono::steady_clock::now() < hard_end) {
        if (C >= 8) {
            const auto alt_begin = chrono::steady_clock::now();
            vector<AssignmentK> alt = make_strong_k4_assignments(A, B,
                    min(hard_end, alt_begin + chrono::milliseconds(120)), base_seed ^ 127u);
            vector<PendingBoard> alt_pool;
            Deadline alt_deadline{min(hard_end, alt_begin + chrono::milliseconds(310))};
            vector<int> rows; for (int d = 4; d <= 10; ++d) rows.push_back(C + d);
            for (int ai = 0; ai < int(alt.size()) && ai < 32 && !alt_deadline.expired(); ++ai) {
                for (int rr : rows) {
                    if (alt_deadline.expired() || rr > C + 20) break;
                    int made = 0;
                    for (int v = 0; v < 56 && !alt_deadline.expired(); ++v) {
                        Board cand;
                        if (!build_split_board(A, alt[ai].dst, M, rr, 4,
                                (base_seed ^ 127u) ^ uint32_t(ai * 10007u + rr * 257u + v * 65537u),
                                alt_deadline, cand)) continue;
                        alt_pool.push_back({(1LL << (rr - C)) + alt[ai].err, move(cand)});
                        if (++made >= 2) break;
                    }
                }
            }
            sort(alt_pool.begin(), alt_pool.end(), [](const PendingBoard& x, const PendingBoard& y) { return x.key < y.key; });
            for (int i = 0; i < int(alt_pool.size()) && i < 3 && chrono::steady_clock::now() < hard_end; ++i) {
                SimResult r = simulate_board(A, B, T, M, alt_pool[i].board);
                if (better_result(r, alt_pool[i].board, best_result, best_board)) { best_result = r; best_board = move(alt_pool[i].board); }
            }
        }

        vector<uint32_t> old_salts = (C <= 7 ? vector<uint32_t>{0u,31u,127u} : vector<uint32_t>{127u});
        for (uint32_t salt : old_salts) {
            if (chrono::steady_clock::now() >= hard_end) break;
            const auto old_queue_end = min(hard_end, chrono::steady_clock::now() + chrono::milliseconds(100));
            vector<QueueAssignment> old_queue = make_queue_assignments(A, B, old_queue_end, base_seed ^ salt);
            int old_simulations = 0;
            for (int qi = 0; qi < int(old_queue.size()) && qi < 4 && chrono::steady_clock::now() < hard_end; ++qi) {
                for (int rr = C + 2; rr <= min(C + 12, C + 20) && chrono::steady_clock::now() < hard_end; ++rr) {
                    for (int v = 0; v < 40 && chrono::steady_clock::now() < hard_end; ++v) {
                        Board qb;
                        if (!build_queue_board(A, old_queue[qi].dst, rr,
                                (base_seed ^ salt) ^ uint32_t(0x0BADC000u + qi * 1009u + rr * 97u + v), qb)) continue;
                        SimResult qr = simulate_board(A, B, T, M, qb);
                        ++old_simulations;
                        if (better_result(qr, qb, best_result, best_board)) { best_result = qr; best_board = move(qb); }
                        if (old_simulations >= 3) goto next_old_salt;
                    }
                }
            }
            next_old_salt:;
        }

        if (chrono::steady_clock::now() < hard_end) {
            const auto balanced_end = min(hard_end, chrono::steady_clock::now() + chrono::milliseconds(250));
            vector<QueueAssignment> balanced = make_balanced_queue_assignments(A, B, balanced_end, base_seed);
            int simulations = 0;
            for (int qi = 0; qi < int(balanced.size()) && qi < 3 && chrono::steady_clock::now() < hard_end; ++qi) {
                for (int rr = C + 2; rr <= min(C + 16, C + 20) && chrono::steady_clock::now() < hard_end; ++rr) {
                    for (int v = 0; v < 24 && chrono::steady_clock::now() < hard_end; ++v) {
                        Board qb;
                        if (!build_queue_board(A, balanced[qi].dst, rr,
                                base_seed ^ uint32_t(0x0BADC000u + qi * 1009u + rr * 97u + v), qb)) continue;
                        SimResult qr = simulate_board(A, B, T, M, qb);
                        ++simulations;
                        if (better_result(qr, qb, best_result, best_board)) { best_result = qr; best_board = move(qb); }
                        if (simulations >= 6) goto balanced_done_new;
                    }
                }
            }
            balanced_done_new:;
        }
    }

    if ((C == 8 || C == 10) && best_result.cost <= 100000 && chrono::steady_clock::now() + chrono::milliseconds(130) < hard_end) {
        const auto now = chrono::steady_clock::now();
        Deadline assignment_deadline{min(hard_end, now + chrono::milliseconds(55))};
        vector<AssignmentK> k3 = make_assignments(A, B, 3, assignment_deadline);
        vector<PendingBoard> pool;
        Deadline construction_deadline{min(hard_end, now + chrono::milliseconds(115))};
        for (int ai = 0; ai < int(k3.size()) && ai < 12 && !construction_deadline.expired(); ++ai) {
            for (int d = 3; d <= 9 && !construction_deadline.expired(); ++d) {
                int made = 0;
                for (int v = 0; v < 40 && !construction_deadline.expired(); ++v) {
                    Board cand;
                    if (!build_split_board(A, k3[ai].dst, M, C + d, 3,
                            base_seed ^ uint32_t(0x165667B1u + ai * 10007u + (C + d) * 257u + v),
                            construction_deadline, cand)) continue;
                    pool.push_back({(1LL << d) + k3[ai].err, move(cand)});
                    if (++made >= 2) break;
                }
            }
        }
        sort(pool.begin(), pool.end(), [](const PendingBoard& x, const PendingBoard& y) { return x.key < y.key; });
        for (int i = 0; i < int(pool.size()) && i < 3 && chrono::steady_clock::now() < hard_end; ++i) {
            SimResult r = simulate_board(A, B, T, M, pool[i].board);
            if (better_result(r, pool[i].board, best_result, best_board)) { best_result = r; best_board = move(pool[i].board); }
        }
    }

    cout<<best_board.R<<"\n";for(int r=0;r<best_board.R;r++){for(int c=0;c<C;c++){if(c)cout<<" ";cout<<best_board.g[r][c];}cout<<"\n";}return 0;
}
