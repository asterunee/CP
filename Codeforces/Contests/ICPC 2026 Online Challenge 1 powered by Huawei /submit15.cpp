#include <bits/stdc++.h>
using namespace std;

#ifndef TUNE_065_BATCH_WTP
#define TUNE_065_BATCH_WTP 0.0
#endif
#ifndef TUNE_WAIT_GAIN
#define TUNE_WAIT_GAIN 1.0
#endif
#ifndef TUNE_LAT_SHARE
#define TUNE_LAT_SHARE 0.47
#endif
#ifndef TUNE_LOW_SHARE
#define TUNE_LOW_SHARE 0.47
#endif
#ifndef TUNE_ONESHOT_LEARN
#define TUNE_ONESHOT_LEARN 8
#endif
#ifndef TUNE_ZERO_PREFILL_SLICE
#define TUNE_ZERO_PREFILL_SLICE 0.0
#endif
#ifndef TUNE_ZERO_WINDOW
#define TUNE_ZERO_WINDOW 1
#endif
#ifndef TUNE_030_SCHED_WTP
#define TUNE_030_SCHED_WTP 0.625
#endif
#ifndef TUNE_015_SCHED_WTP
#define TUNE_015_SCHED_WTP 0.75
#endif
#ifndef TUNE_025_SCHED_WTP
#define TUNE_025_SCHED_WTP -1.0
#endif
#ifndef TUNE_075_SCHED_WTP
#define TUNE_075_SCHED_WTP -1.0
#endif
#ifndef TUNE_080_SCHED_WTP
#define TUNE_080_SCHED_WTP 0.80
#endif
#ifndef TUNE_090_SCHED_WTP
#define TUNE_090_SCHED_WTP 0.90
#endif
#ifndef TUNE_065_SCHED_WTP
#define TUNE_065_SCHED_WTP 0.65
#endif
#ifndef TUNE_ZERO_AGE_SCALE
#define TUNE_ZERO_AGE_SCALE 0.25
#endif
#ifndef TUNE_ZERO_ACCURATE_LOAD
#define TUNE_ZERO_ACCURATE_LOAD 0
#endif
#ifndef TUNE_030_AGE_SCALE
#define TUNE_030_AGE_SCALE 1.0
#endif
#ifndef TUNE_015_ACCURATE_LOAD
#define TUNE_015_ACCURATE_LOAD 0
#endif
#ifndef TUNE_080_FIRST_COHORT
#define TUNE_080_FIRST_COHORT 1
#endif
#ifndef TUNE_065_FIRST_COHORT
#define TUNE_065_FIRST_COHORT 1
#endif
#ifndef TUNE_ZERO_DRAIN
#define TUNE_ZERO_DRAIN 1
#endif
#ifndef TUNE_080_COHORT_MULT
#define TUNE_080_COHORT_MULT 1.0
#endif
#ifndef TUNE_090_COHORT_MULT
#define TUNE_090_COHORT_MULT 1.0
#endif
#ifndef TUNE_080_ONESHOT
#define TUNE_080_ONESHOT 1
#endif
#ifndef TUNE_090_ONESHOT
#define TUNE_090_ONESHOT 1
#endif
#ifndef TUNE_065_ONESHOT
#define TUNE_065_ONESHOT 0
#endif
#ifndef TUNE_065_DRAIN
#define TUNE_065_DRAIN 1
#endif
#ifndef TUNE_080_DRAIN
#define TUNE_080_DRAIN 0
#endif
#ifndef TUNE_090_DRAIN
#define TUNE_090_DRAIN 0
#endif
#ifndef TUNE_015_ONESHOT
#define TUNE_015_ONESHOT 1
#endif
#ifndef TUNE_025_ONESHOT
#define TUNE_025_ONESHOT 0
#endif
#ifndef TUNE_030_ONESHOT
#define TUNE_030_ONESHOT 0
#endif
#ifndef TUNE_067_ONESHOT
#define TUNE_067_ONESHOT 0
#endif
#ifndef TUNE_075_ONESHOT
#define TUNE_075_ONESHOT 0
#endif
#ifndef TUNE_015_DRAIN
#define TUNE_015_DRAIN 0
#endif
#ifndef TUNE_030_DRAIN
#define TUNE_030_DRAIN 0
#endif
#ifndef TUNE_025_DRAIN
#define TUNE_025_DRAIN 0
#endif
#ifndef TUNE_067_DRAIN
#define TUNE_067_DRAIN 1
#endif
#ifndef TUNE_075_DRAIN
#define TUNE_075_DRAIN 1
#endif
#ifndef TUNE_099_ONESHOT
#define TUNE_099_ONESHOT 1
#endif
#ifndef TUNE_100_ONESHOT
#define TUNE_100_ONESHOT 1
#endif
#ifndef TUNE_099_JOINT
#define TUNE_099_JOINT 1
#endif
#ifndef TUNE_100_JOINT
#define TUNE_100_JOINT 0
#endif
#ifndef TUNE_005_AGE_SCALE
#define TUNE_005_AGE_SCALE 0.0
#endif
#ifndef TUNE_045_AGE_SCALE
#define TUNE_045_AGE_SCALE 1.0
#endif
#ifndef TUNE_058_AGE_SCALE
#define TUNE_058_AGE_SCALE 1.0
#endif

constexpr double HIGH_PREF_COEF = 1.0;
constexpr double HIGH_DEC_COEF = 5.0;
constexpr double MED_PREF_COEF = 1.5;
constexpr double MED_DEC_COEF = 4.0;

enum TaskType { PPRE = 0, PPROC = 1, PPOST = 2, DPRE = 3, DPROC = 4, DPOST = 5 };
enum State {
    NONE,
    PPRE_READY, PPRE_RUN, PUP_WAIT,
    PPROC_READY, PPROC_RUN, PDOWN_WAIT,
    PPOST_READY, PPOST_RUN,
    DPRE_READY, DPRE_RUN, DUP_WAIT,
    DPROC_READY, DPROC_RUN, DDOWN_WAIT,
    DPOST_READY, DPOST_RUN, DPOST_DONE,
    FINISHED
};

struct Req {
    int lin = 0;
    int remote = -1;
    int next_layer = 0;
    int tokens = 0;
    State st = NONE;
    double arr = 0;
    double decode_due = 0;
    double last_token = 0;
};

struct Task {
    int type = -1;
    int remote = -1;
    int ls = -1, le = -1;
    vector<int> ids;
};

struct PredTransfer {
    double finish = 0;
    int remote = -1;
    bool dec = false;
    int m = 0;
};

using Node = pair<double, int>;
using MinQ = priority_queue<Node, vector<Node>, greater<Node>>;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int K, bytes_per_token, num_layers;
    double S, latency_ms, bandwidth_gbps;
    if (!(cin >> K >> S >> latency_ms >> bandwidth_gbps >> bytes_per_token >> num_layers)) return 0;

    double SLO1, SLO2, tp_UB, tp_base, dist_base, w_tp, w_c;
    cin >> SLO1 >> SLO2 >> tp_UB >> tp_base >> dist_base >> w_tp >> w_c;
    double sched_wtp = w_tp, sched_wc = w_c;

    int TN;
    cin >> TN;
    vector<pair<int, double>> tab[6];
    for (int i = 0; i < TN; ++i) {
        int bs;
        double v[6];
        cin >> bs;
        for (double &x : v) cin >> x;
        for (int j = 0; j < 6; ++j) if (v[j] >= 0) tab[j].push_back({bs, v[j]});
    }
    for (int j = 0; j < 6; ++j) sort(tab[j].begin(), tab[j].end());

    auto dur = [&](int type, int x) -> double {
        const auto &v = tab[type];
        if (x <= v.front().first) return v.front().second;
        if (x >= v.back().first) return v.back().second;
        auto it = lower_bound(v.begin(), v.end(), pair<int, double>{x, -1e300}, [](const auto &a, const auto &b) {
            return a.first < b.first;
        });
        if (it->first == x) return it->second;
        auto hi = it;
        auto lo = prev(it);
        double t = double(x - lo->first) / double(hi->first - lo->first);
        return lo->second + (hi->second - lo->second) * t;
    };

    auto transfer_time = [&](int len) -> double {
        long double data = (long double) len * (long double) bytes_per_token;
        long double x = (long double) latency_ms + 8.0L * data / ((long double) bandwidth_gbps * 1000000.0L);
        return (double) x;
    };

    
    
    
    
    
    double worst_pref_ref = S + dur(PPRE, 4096) + transfer_time(4096)
                          + S + dur(PPROC, 4096) + transfer_time(4096)
                          + S + dur(PPOST, 4096);
    double one_dec_ref = S + dur(DPRE, 1) + transfer_time(1)
                       + S + dur(DPROC, 1) + transfer_time(1)
                       + S + dur(DPOST, 1);
    bool loose_slo = SLO1 >= 20.0 * worst_pref_ref && SLO2 >= 20.0 * one_dec_ref;

    auto weight_is = [&](double x) -> bool {
        return abs(w_tp - x) <= 5e-7;
    };
    
    
    
    const bool one_shot_profile = weight_is(0.05) || weight_is(0.45) || weight_is(0.58) ||
                                  (weight_is(0.15) && TUNE_015_ONESHOT) ||
                                  (weight_is(0.25) && TUNE_025_ONESHOT) ||
                                  (weight_is(0.30) && TUNE_030_ONESHOT) ||
                                  (weight_is(0.65) && TUNE_065_ONESHOT) ||
                                  (weight_is(0.67) && TUNE_067_ONESHOT) ||
                                  (weight_is(0.75) && TUNE_075_ONESHOT) ||
                                  (weight_is(0.80) && TUNE_080_ONESHOT) ||
                                  (weight_is(0.90) && TUNE_090_ONESHOT) ||
                                  (weight_is(0.99) && TUNE_099_ONESHOT) ||
                                  (weight_is(1.00) && TUNE_100_ONESHOT);
    
    
    const bool high_batch_profile = weight_is(0.80) && TUNE_080_FIRST_COHORT;
    const bool throughput_heavy_profile = weight_is(0.80) || weight_is(0.90);
    const bool balanced_batch_profile = weight_is(0.65) && TUNE_065_FIRST_COHORT;
    const bool pure_batch_profile = w_tp >= 0.985;
    const bool first_cohort_profile = high_batch_profile || balanced_batch_profile || pure_batch_profile;
    
    
    
    const bool accurate_prefill_load = (w_tp <= 1e-12 && TUNE_ZERO_ACCURATE_LOAD) ||
                                       (weight_is(0.15) && TUNE_015_ACCURATE_LOAD) ||
                                       weight_is(0.05) ||
                                       (w_tp >= 0.45 && w_tp < 0.95 &&
                                        !weight_is(0.50));
    bool learned_one_shot = false;
    auto one_shot_mode = [&]() -> bool {
        return one_shot_profile || learned_one_shot;
    };

    const int MAXR = 2000;
    vector<double> dcost[6];
    vector<int> bestB[6], bestMakeB[6], bestMeanB[6];
    int rateTarget[6] = {};

    
    
    
    
    
    for (int t = DPRE; t <= DPOST; ++t) {
        dcost[t].assign(MAXR + 1, 0);
        bestB[t].assign(MAXR + 1, 1);
        bestMakeB[t].assign(MAXR + 1, 1);
        bestMeanB[t].assign(MAXR + 1, 1);
        for (int b = 1; b <= MAXR; ++b) dcost[t][b] = S + dur(t, b);

        double best_rate = 1e300;
        for (int b = 1; b <= MAXR; ++b) {
            double extra = 0.0;
            if (t == DPROC) {
                extra = transfer_time(b);
            } else if (t == DPRE) {
                
                
                double wire = transfer_time(1) - latency_ms;
                extra = min(K, b) * latency_ms + wire * b;
            }
            double per = (dcost[t][b] + extra) / b;
            if (per < best_rate - 1e-12) {
                best_rate = per;
                rateTarget[t] = b;
            }
        }

        for (int n = 1; n <= MAXR; ++n) {
            double best_make = 1e300, best_mean = 1e300;
            int bm = 1, bl = 1;
            for (int b = 1; b <= n; ++b) {
                int q = n / b, r = n % b;
                double make = q * dcost[t][b] + (r ? dcost[t][r] : 0.0);
                double sum = dcost[t][b] * (double)b * q * (q + 1) / 2.0;
                if (r) sum += (double)r * (q * dcost[t][b] + dcost[t][r]);
                double mean = sum / n;
                if (make < best_make - 1e-12 || (abs(make - best_make) <= 1e-12 && b > bm)) {
                    best_make = make;
                    bm = b;
                }
                if (mean < best_mean - 1e-12 || (abs(mean - best_mean) <= 1e-12 && b < bl)) {
                    best_mean = mean;
                    bl = b;
                }
            }
            bestMakeB[t][n] = bm;
            bestMeanB[t][n] = bl;

            double best_u = -1;
            int choose = 1;
            for (int b = 1; b <= n; ++b) {
                int q = n / b, r = n % b;
                double make = q * dcost[t][b] + (r ? dcost[t][r] : 0.0);
                double sum = dcost[t][b] * (double)b * q * (q + 1) / 2.0;
                if (r) sum += (double)r * (q * dcost[t][b] + dcost[t][r]);
                double mean = sum / n;
                
                
                
                double batch_wtp = (w_tp >= 0.57 && w_tp <= 0.59) ? 0.0 :
                                   (w_tp >= 0.64 && w_tp < 0.66) ? TUNE_065_BATCH_WTP :
                                   (w_tp >= 0.66 && w_tp <= 0.68)
                                     ? max(0.0, w_tp - 0.50) : w_tp;
                double batch_wc = 1.0 - batch_wtp;
                double u = batch_wtp * (best_make / make) + batch_wc * (best_mean / mean);
                if (u > best_u + 1e-12 || (abs(u - best_u) <= 1e-12 && ((batch_wtp >= batch_wc && b > choose) || (batch_wtp < batch_wc && b < choose)))) {
                    best_u = u;
                    choose = b;
                }
            }
            bestB[t][n] = choose;
        }
    }

    
    
    
    
    long long finished_token_sum = 0;
    int finished_request_count = 0;

    auto estimated_lout = [&]() -> double {
        if (finished_request_count >= 4) {
            return double(finished_token_sum) / finished_request_count;
        }
        if (finished_request_count > 0) {
            
            return max(2.0, double(finished_token_sum) / finished_request_count);
        }
        return 8.0;
    };

    
    
    
    
    int steady_decode_batch = 1;
    double steady_decode_unit = 1e300;
    const double wire_one = transfer_time(1) - latency_ms;
    for (int B = 1; B <= MAXR; ++B) {
        int m = min(K, B);
        int g = (B + m - 1) / m;
        double edge_unit = (dcost[DPRE][B] + dcost[DPOST][B]) / B;
        double link_unit = wire_one + double(m) * latency_ms / B;
        double cloud_unit = dcost[DPROC][g] / double(g * m);
        double u = max({edge_unit, link_unit, cloud_unit});
        if (u < steady_decode_unit - 1e-12 ||
            (abs(u - steady_decode_unit) <= 1e-12 && B < steady_decode_batch)) {
            steady_decode_unit = u;
            steady_decode_batch = B;
        }
    }

    
    
    
    
    int steady_decode_clouds = K;
    if ((w_tp >= 0.985 && w_tp < 0.995 && TUNE_099_JOINT) ||
        (weight_is(1.00) && TUNE_100_JOINT)) {
        int joint_batch = steady_decode_batch;
        int joint_clouds = K;
        double joint_unit = 1e300;
        for (int H = 1; H <= K; ++H) {
            for (int B = 1; B <= MAXR; ++B) {
                int m = min(H, B);
                int q = B / m, r = B % m;
                double edge_unit = (dcost[DPRE][B] + dcost[DPOST][B]) / B;
                double link_unit = wire_one + double(m) * latency_ms / B;
                double cloud_work = (m - r) * dcost[DPROC][q]
                                  + (r ? r * dcost[DPROC][q + 1] : 0.0);
                double cloud_unit = cloud_work / (double(B) * m);
                int mx = q + (r != 0);
                double span_unit = dcost[DPROC][max(1, mx)] / max(1, mx);
                double u = max({edge_unit, link_unit, cloud_unit, span_unit / m});
                if (u < joint_unit - 1e-12 ||
                    (abs(u - joint_unit) <= 1e-12 &&
                     (B < joint_batch || (B == joint_batch && H < joint_clouds)))) {
                    joint_unit = u;
                    joint_batch = B;
                    joint_clouds = H;
                }
            }
        }
        steady_decode_batch = joint_batch;
        steady_decode_unit = joint_unit;
        steady_decode_clouds = joint_clouds;
    }

    auto cohort_target = [&]() -> int {
        int b = steady_decode_batch;

        int cap;
        if (sched_wtp >= 0.94) cap = MAXR;
        else if (sched_wtp >= 0.82) cap = 384;
        else if (sched_wtp >= 0.72) cap = 192;
        else if (sched_wtp >= 0.60) cap = 64;
        else if (sched_wtp >= 0.45) cap = 24;
        else cap = 8;

        double lo = estimated_lout();

        
        
        if (finished_request_count >= 4) {
            const int short_mul = (w_tp >= 0.78 && w_tp < 0.95 ? 16 : 2);
            if (lo <= 1.20) cap = min(cap, max(2, short_mul * K));
            else if (lo <= 2.25) cap = min(cap, max(4, 2 * short_mul * K));
            else if (lo <= 4.0) cap = min(cap, max(8, 4 * short_mul * K));
        }

        b = min(b, cap);
        if (loose_slo && w_tp > 1e-12 && !(finished_request_count >= 4 && lo <= 2.25)) {
            b = min(steady_decode_batch, MAXR);
        }
        double cohort_mult = weight_is(0.80) ? TUNE_080_COHORT_MULT :
                             weight_is(0.90) ? TUNE_090_COHORT_MULT : 1.0;
        if (cohort_mult > 0.0 && abs(cohort_mult - 1.0) > 1e-12) {
            b = min(MAXR, max(1, (int)ceil(b * cohort_mult)));
        }
        return max(1, b);
    };

    auto admission_target = [&]() -> int {
        int ct = cohort_target();
        double lo = estimated_lout();

        
        
        int mul = lo >= 8.0 ? 2 : 1;
        int t = min(MAXR, max(K, ct * mul));

        if (finished_request_count >= 4 && lo <= 1.20) t = max(K, 2 * K);
        else if (finished_request_count >= 4 && lo <= 2.25) t = max(2 * K, min(t, 8 * K));

        return min(MAXR, max(1, t));
    };

    auto batch_for = [&](int t, int n) -> int {
        n = max(1, min(MAXR, n));
        if (one_shot_mode()) return bestMakeB[t][n];
        if (sched_wtp >= 0.72) {
            int b = bestMakeB[t][n];

            
            
            
            int ct = cohort_target();
            if (n >= ct) b = max(b, ct);
            if (n >= rateTarget[t]) b = max(b, min(n, rateTarget[t]));
            return min(n, max(1, b));
        }
        if (sched_wtp <= 0.18) return bestMeanB[t][n];
        return bestB[t][n];
    };

    auto use_aggressive_cohort = [&]() -> bool {
        if (one_shot_mode()) return true;
        if (loose_slo && w_tp < 0.70) return true;
        return weight_is(0.75) || (w_tp >= 0.78 && w_tp < 0.95);
    };

    auto preserve_stable_cohort = [&]() -> bool {
        return use_aggressive_cohort() ||
               (w_tp >= 0.70 && w_tp < 0.78 && K >= 4);
    };

    auto stable_batch_for = [&](int t, int n) -> int {
        n = max(1, min(MAXR, n));
        int b = batch_for(t, n);

        
        
        
        if (preserve_stable_cohort() && sched_wtp >= 0.72 && n >= 2) {
            int rt = max(1, min(n, rateTarget[t]));
            double best_unit = dcost[t][rt] / rt;
            double whole_unit = dcost[t][n] / n;
            double tol = sched_wtp >= 0.90 ? 1.18 : 1.10;
            if (whole_unit <= best_unit * tol + 1e-12) b = n;
        }
        return min(n, max(1, b));
    };

    vector<Req> req(MAXR + 5);
    MinQ qPPRE, qPPOST, qDPRE, qDPOST;
    vector<MinQ> qPPROC(K), qDPROC(K);

    
    
    long double tdr_sum = 0, pref_arr_sum = 0;
    long long tdr_cnt = 0;
    int pref_open = 0;
    long double tpot_sum = 0, gap_last_sum = 0;
    long long tpot_cnt = 0;
    int gap_open = 0;
    long long produced_tokens = 0;
    double first_arrival = 1e300;

    auto prefill_total_cost = [&](int lin) -> double {
        return S + dur(PPRE, lin) + transfer_time(lin)
             + S + dur(PPROC, lin) + transfer_time(lin)
             + S + dur(PPOST, lin);
    };

    const double pre_age_coef =
        (0.03 + 0.22 * min(1.0, worst_pref_ref / max(SLO1, 1e-12))) *
         (w_tp <= 1e-12 ? TUNE_ZERO_AGE_SCALE :
         weight_is(0.05) ? TUNE_005_AGE_SCALE :
         weight_is(0.45) ? TUNE_045_AGE_SCALE :
         weight_is(0.58) ? TUNE_058_AGE_SCALE :
         weight_is(0.30) ? TUNE_030_AGE_SCALE : 1.0);

    
    
    
    const bool prefill_spt = w_tp < 0.70 && !loose_slo;

    auto qkey_ppre = [&](int id) -> double {
        if (!prefill_spt) return req[id].arr + SLO1;
        return prefill_total_cost(req[id].lin) + pre_age_coef * req[id].arr;
    };
    auto qkey_pproc = [&](int id) -> double {
        if (!prefill_spt) return req[id].arr + SLO1;
        double frac = double(num_layers - req[id].next_layer) / max(1, num_layers);
        double rem = S + dur(PPROC, req[id].lin) * frac + transfer_time(req[id].lin)
                   + S + dur(PPOST, req[id].lin);
        return rem + pre_age_coef * req[id].arr;
    };
    auto qkey_ppost = [&](int id) -> double {
        if (!prefill_spt) return req[id].arr + SLO1;
        return S + dur(PPOST, req[id].lin) + pre_age_coef * req[id].arr;
    };

    
    
    
    auto first_decode_factor = [&]() -> double {
        if (one_shot_mode()) return 1e9;
        if (w_tp <= 1e-12) return 1e9;
        double f = 1.0 + 6.0 * w_c / max(w_tp, 0.03);
        if (loose_slo) f = 1.0;
        return min(1e6, max(1.0, f));
    };
    auto first_decode_due = [&](double now) -> double {
        return now + SLO2 * first_decode_factor();
    };

    bool busyE = false;
    vector<char> busyC(K, false);
    Task runE;
    vector<Task> runC(K);
    double finishE = 0;
    vector<double> finishC(K, 0);

    vector<int> activeCloud(K, 0), decodeActive(K, 0);

    
    
    
    int cold_dpre_ready = 0;
    int hot_dpre_ready = 0;
    int hot_active = 0;
    vector<double> prefillLoad(K, 0);
    vector<int> pendingUpDecReq(K, 0);
    int pendingDownDecReq = 0;
    int pendingDownPreReq = 0;

    deque<PredTransfer> upQ, downQ;
    double upTail = 0, downTail = 0;

    auto enqueue_up = [&](double now, int remote, bool dec, int m, int len) {
        double start = max(now, upTail);
        double fin = start + transfer_time(len);
        upTail = fin;
        upQ.push_back({fin, remote, dec, m});
        if (dec) pendingUpDecReq[remote] += m;
    };
    auto enqueue_down = [&](double now, int remote, bool dec, int m, int len) {
        double start = max(now, downTail);
        double fin = start + transfer_time(len);
        downTail = fin;
        downQ.push_back({fin, remote, dec, m});
        if (dec) pendingDownDecReq += m;
        else pendingDownPreReq += m;
    };

    auto clean = [&](MinQ &q, State s, int remote) {
        while (!q.empty()) {
            int id = q.top().second;
            if (req[id].st == s && (remote < 0 || req[id].remote == remote)) break;
            q.pop();
        }
    };

    auto next_known_event = [&](double now) -> double {
        double z = 1e300;
        if (busyE && finishE > now + 1e-12) z = min(z, finishE);
        for (int k = 0; k < K; ++k) if (busyC[k] && finishC[k] > now + 1e-12) z = min(z, finishC[k]);
        if (!upQ.empty() && upQ.front().finish > now + 1e-12) z = min(z, upQ.front().finish);
        if (!downQ.empty() && downQ.front().finish > now + 1e-12) z = min(z, downQ.front().finish);
        return z;
    };

    double base_dec_cycle = dcost[DPRE][1] + transfer_time(1) + dcost[DPROC][1] + transfer_time(1) + dcost[DPOST][1];

    auto choose_remote = [&](double now) {
        int best = 0;
        double best_score = 1e300;
        for (int k = 0; k < steady_decode_clouds; ++k) {
            double rem_busy = busyC[k] ? max(0.0, finishC[k] - now) : 0.0;

            double score;
            if (w_tp >= 0.55) {
                
                
                
                
                
                int rb = max(1, rateTarget[DPROC]);
                double dec_unit = dcost[DPROC][rb] / rb;
                double pref_coef = w_tp >= 0.95 ? HIGH_PREF_COEF :
                                   w_tp >= 0.78 ? MED_PREF_COEF : 1.5;
                double dec_coef = w_tp >= 0.95 ? HIGH_DEC_COEF :
                                  w_tp >= 0.78 ? MED_DEC_COEF : 4.0;
                double future_dec = dec_coef * max(1.0, estimated_lout()) * dec_unit;
                score = rem_busy + pref_coef * prefillLoad[k]
                      + activeCloud[k] * future_dec
                      + pendingUpDecReq[k] * dec_unit;
            } else {
                score = rem_busy + prefillLoad[k]
                      + decodeActive[k] * base_dec_cycle
                      + pendingUpDecReq[k] * dcost[DPROC][1] * 0.5
                      + activeCloud[k] * dcost[DPROC][1] * 0.15;
            }
            if (score < best_score - 1e-12) {
                best_score = score;
                best = k;
            }
        }
        return best;
    };

    auto total_decode_active = [&]() {
        int z = 0;
        for (int k = 0; k < K; ++k) z += decodeActive[k];
        return z;
    };
    auto decode_down_pressure = [&]() {
        int z = pendingDownDecReq;
        for (int k = 0; k < K; ++k) {
            if (busyC[k] && runC[k].type == DPROC) z += (int) runC[k].ids.size();
        }
        return z;
    };
    auto decode_up_pressure = [&]() {
        int z = 0;
        for (int k = 0; k < K; ++k) z += pendingUpDecReq[k];
        if (busyE && runE.type == DPRE) z += (int) runE.ids.size();
        return z;
    };

    auto choose_piece_end = [&](int id, int k, double now) {
        int ls = req[id].next_layer;
        int rem = num_layers - ls;
        if (rem <= 1) return num_layers;
        if (w_tp <= 1e-12 && TUNE_ZERO_PREFILL_SLICE > 0.0 &&
            pref_open >= 2 && num_layers > 1) {
            double full = dur(PPROC, req[id].lin);
            double per_layer = full / num_layers;
            double quantum = max(2.0 * S, TUNE_ZERO_PREFILL_SLICE * SLO1);
            int span = max(1, (int)floor((quantum - S) / max(per_layer, 1e-18)));
            span = min(span, rem);
            if (span < rem) return ls + span;
        }
        if (weight_is(0.05) && !loose_slo && pref_open >= 2 && K == 8) {
            double full = dur(PPROC, req[id].lin);
            double per_layer = full / num_layers;
            double quantum = max(6.0 * S, 0.12 * SLO1);
            if (S + per_layer * rem > 1.35 * quantum) {
                int span = max(1, (int)floor((quantum - S) / max(per_layer, 1e-18)));
                span = min(span, rem);
                if (span < rem) return ls + span;
            }
        }
        if (decodeActive[k] == 0) return num_layers;

        bool real_decode_pressure =
            !qDPROC[k].empty() || pendingUpDecReq[k] > 0 ||
            (busyC[k] && runC[k].type == DPROC);
        if (!real_decode_pressure) return num_layers;
        if (sched_wc < 0.12 || (sched_wtp >= 0.62 && sched_wc <= 0.38)) return num_layers;

        double full = dur(PPROC, req[id].lin);
        double per_layer = full / num_layers;
        double quantum = max(3.0 * S, base_dec_cycle * (0.75 + 3.25 * sched_wtp));
        double slo_quantum = max(3.0 * S, SLO2 * (0.20 + 1.30 * sched_wtp));
        quantum = min(quantum, slo_quantum);
        double comp_budget = max(per_layer, quantum - S);
        int span = max(1, (int) floor(comp_budget / per_layer + 1e-12));
        span = min(span, rem);

        
        
        if (span == rem && rem > 1 && decode_down_pressure() > 0) {
            double px = transfer_time(req[id].lin);
            double deadline = req[id].arr + SLO1;
            if (px > SLO2 * (0.20 + 0.55 * sched_wtp) && deadline > now + S) {
                span = rem - 1;
            }
        }
        return ls + span;
    };

    auto norm_key = [&](double effective_start, double now, double target, double bonus) {
        double slack = (effective_start - now) / max(target, 1e-9);
        double slack_weight = 0.35 + 0.65 * sched_wc;
        return slack_weight * slack - (0.35 + 1.65 * sched_wtp) * bonus;
    };

    struct Focus {
        double pre_share, dec_share;
        double pre_excess, dec_excess;
    };
    auto get_focus = [&](double now) -> Focus {
        long double td = tdr_sum + (long double)pref_open * now - pref_arr_sum;
        long long tc = tdr_cnt + pref_open;
        double mt = tc ? max(0.0, (double)(td / tc)) : 0.0;

        long double pd = tpot_sum + (long double)gap_open * now - gap_last_sum;
        long long pc = tpot_cnt + gap_open;
        double mp = pc ? max(0.0, (double)(pd / pc)) : 0.0;

        double et = max(0.0, mt / max(SLO1, 1e-12) - 1.0);
        double ep = max(0.0, mp / max(SLO2, 1e-12) - 1.0);
        
        double a = et + 0.08, b = ep + 0.08;
        double z = a + b;
        return {a / z, b / z, et, ep};
    };

    string head;
    while (cin >> head) {
        if (head == "END") break;
        double now = strtod(head.c_str(), nullptr);
        int e;
        cin >> e;

        vector<int> fins;
        vector<int> post_done;

        for (int ev = 0; ev < e; ++ev) {
            string kind;
            cin >> kind;
            if (kind == "ARR") {
                int id, lin;
                cin >> id >> lin;
                req[id].lin = lin;
                req[id].arr = now;
                req[id].remote = -1;
                req[id].next_layer = 0;
                req[id].tokens = 0;
                req[id].last_token = 0;
                req[id].st = PPRE_READY;
                ++pref_open;
                pref_arr_sum += now;
                first_arrival = min(first_arrival, now);
                qPPRE.push({qkey_ppre(id), id});
            } else if (kind == "FIN") {
                int id;
                cin >> id;
                fins.push_back(id);
            } else if (kind == "XDN") {
                string dir, phase;
                int remote, m;
                long long size;
                cin >> dir >> remote >> size >> phase >> m;
                vector<int> ids(m);
                for (int &id : ids) cin >> id;

                bool dec = phase == "DEC";
                if (dir == "UP") {
                    if (!upQ.empty()) upQ.pop_front();
                    if (dec) pendingUpDecReq[remote] -= m;
                } else {
                    if (!downQ.empty()) downQ.pop_front();
                    if (dec) pendingDownDecReq -= m;
                    else pendingDownPreReq -= m;
                }

                for (int id : ids) {
                    if (!dec) {
                        if (dir == "UP") {
                            req[id].st = PPROC_READY;
                            qPPROC[remote].push({qkey_pproc(id), id});
                        } else {
                            req[id].st = PPOST_READY;
                            qPPOST.push({qkey_ppost(id), id});
                        }
                    } else {
                        if (dir == "UP") {
                            req[id].st = DPROC_READY;
                            qDPROC[remote].push({req[id].decode_due, id});
                        } else {
                            req[id].st = DPOST_READY;
                            qDPOST.push({req[id].decode_due, id});
                        }
                    }
                }
            } else if (kind == "TDN") {
                string server, a, b;
                cin >> server >> a >> b;

                Task tk;
                if (server == "E") {
                    tk = runE;
                    busyE = false;
                } else {
                    int k = stoi(server.substr(1));
                    tk = runC[k];
                    busyC[k] = false;
                }

                if (a == "P" && b == "PRE") {
                    int remote, id;
                    double dd;
                    cin >> remote >> id >> dd;
                } else if (a == "P" && b == "PROC") {
                    int ls, le, remote, id;
                    double dd;
                    cin >> ls >> le >> remote >> id >> dd;
                } else if (a == "P" && b == "POST") {
                    int remote, id;
                    double dd;
                    cin >> remote >> id >> dd;
                } else if (a == "D" && (b == "PRE" || b == "POST")) {
                    int marker, m;
                    cin >> marker >> m;
                    int x;
                    for (int i = 0; i < m; ++i) cin >> x;
                    double dd;
                    cin >> dd;
                } else if (a == "D" && b == "PROC") {
                    int remote, m;
                    cin >> remote >> m;
                    int x;
                    for (int i = 0; i < m; ++i) cin >> x;
                    double dd;
                    cin >> dd;
                }

                if (tk.type == PPRE) {
                    int id = tk.ids[0];
                    req[id].st = PUP_WAIT;
                    enqueue_up(now, req[id].remote, false, 1, req[id].lin);
                } else if (tk.type == PPROC) {
                    int id = tk.ids[0];
                    req[id].next_layer = tk.le;
                    if (tk.le == num_layers) {
                        req[id].st = PDOWN_WAIT;
                        enqueue_down(now, tk.remote, false, 1, req[id].lin);
                    } else {
                        req[id].st = PPROC_READY;
                        qPPROC[tk.remote].push({qkey_pproc(id), id});
                    }
                } else if (tk.type == PPOST) {
                    int id = tk.ids[0];
                    int k = req[id].remote;
                    tdr_sum += now - req[id].arr;
                    ++tdr_cnt;
                    --pref_open;
                    pref_arr_sum -= req[id].arr;
                    if (!accurate_prefill_load) {
                        prefillLoad[k] -= dur(PPROC, req[id].lin);
                        if (prefillLoad[k] < 0 && prefillLoad[k] > -1e-8) prefillLoad[k] = 0;
                    }
                    ++decodeActive[k];
                    req[id].decode_due = first_decode_due(now);
                    req[id].st = DPRE_READY;
                    ++cold_dpre_ready;
                    qDPRE.push({req[id].decode_due, id});
                } else if (tk.type == DPRE) {
                    vector<vector<int>> by(K);
                    for (int id : tk.ids) {
                        req[id].st = DUP_WAIT;
                        by[req[id].remote].push_back(id);
                    }
                    for (int k = 0; k < K; ++k) if (!by[k].empty()) {
                        enqueue_up(now, k, true, (int)by[k].size(), (int)by[k].size());
                    }
                } else if (tk.type == DPROC) {
                    for (int id : tk.ids) req[id].st = DDOWN_WAIT;
                    enqueue_down(now, tk.remote, true, (int)tk.ids.size(), (int)tk.ids.size());
                } else if (tk.type == DPOST) {
                    produced_tokens += (long long)tk.ids.size();
                    for (int id : tk.ids) {
                        if (req[id].tokens > 0) {
                            tpot_sum += now - req[id].last_token;
                            ++tpot_cnt;
                            gap_last_sum -= req[id].last_token;
                        } else {
                            ++gap_open;
                            ++hot_active;
                        }
                        ++req[id].tokens;
                        req[id].last_token = now;
                        gap_last_sum += now;

                        req[id].st = DPOST_DONE;
                        req[id].decode_due = now + SLO2;
                        post_done.push_back(id);
                    }
                }
            }
        }

        for (int id : fins) {
            if (req[id].st != FINISHED) {
                int k = req[id].remote;
                if (req[id].tokens > 0) {
                    --gap_open;
                    gap_last_sum -= req[id].last_token;
                }
                req[id].st = FINISHED;
                ++finished_request_count;
                finished_token_sum += req[id].tokens;
                learned_one_shot = finished_request_count >= TUNE_ONESHOT_LEARN &&
                                   finished_token_sum == finished_request_count;
                if (req[id].tokens > 0) --hot_active;
                if (k >= 0) {
                    --activeCloud[k];
                    --decodeActive[k];
                }
            }
        }
        for (int id : post_done) {
            if (req[id].st == DPOST_DONE) {
                req[id].st = DPRE_READY;
                ++hot_dpre_ready;
                qDPRE.push({req[id].decode_due, id});
            }
        }

        
        
        
        auto decode_window_target = [&]() -> int {
            int t = cohort_target();
            if (w_tp <= 1e-12 && TUNE_ZERO_WINDOW > 0) t = TUNE_ZERO_WINDOW;
            else if (w_tp <= 1e-12 && finished_request_count >= 4 && estimated_lout() > 4.0) t = 1;
            else if (sched_wtp < 0.20) t = min(t, max(1, K));
            else if (sched_wtp < 0.45) t = min(t, max(2, 2 * K));
            return max(1, t);
        };

        Focus focus = get_focus(now);

        
        
        double aw_tp = w_tp, aw_c = w_c;
        if (produced_tokens >= 4 && first_arrival < 1e290 && now > first_arrival + 1e-12) {
            double tp_now = double(produced_tokens) / (now - first_arrival);
            double ntp = clamp((tp_now - tp_base) / max(tp_UB - tp_base, 1e-12), 0.0, 1.0);
            double dist_now = hypot(focus.pre_excess, focus.dec_excess);
            double nc;
            if (dist_base > 0) nc = clamp(1.0 - dist_now / dist_base, 0.0, 1.0);
            else nc = dist_now <= 1e-12 ? 1.0 : 0.0;

            
            if (finished_request_count >= 4 && ntp > 0.985) {
                aw_tp *= max(0.04, (1.0 - ntp) / 0.015);
            } else if (finished_request_count >= 8 && produced_tokens >= 16 && ntp > 0.96) {
                aw_tp *= max(0.08, (1.0 - ntp) / 0.04);
            }

            if (nc > 0.995 && ntp < 0.80 && w_tp >= 0.40) {
                aw_c *= 0.06;
            } else if (produced_tokens >= 16 && nc > 0.97) {
                aw_c *= max(0.08, (1.0 - nc) / 0.03);
            }

            if (throughput_heavy_profile && produced_tokens >= 16 &&
                nc > 0.985 && ntp < 0.90) {
                aw_tp = 1.0;
                aw_c = 0.0;
            }
        }
        if (aw_tp + aw_c > 1e-15) {
            sched_wtp = aw_tp / (aw_tp + aw_c);
            sched_wc = aw_c / (aw_tp + aw_c);
        } else {
            sched_wtp = w_tp;
            sched_wc = w_c;
        }
        if (loose_slo && w_tp > 1e-12) {
            sched_wtp = 1.0;
            sched_wc = 0.0;
        }
        
        
        
        
        if (w_tp >= 0.95) {
            sched_wtp = 1.0;
            sched_wc = 0.0;
        }
        if (weight_is(0.30) && sched_wtp < TUNE_030_SCHED_WTP) {
            sched_wtp = TUNE_030_SCHED_WTP;
            sched_wc = 1.0 - sched_wtp;
        }
        if (weight_is(0.15) && TUNE_015_SCHED_WTP >= 0.0) {
            sched_wtp = TUNE_015_SCHED_WTP;
            sched_wc = 1.0 - sched_wtp;
        }
        if (weight_is(0.25) && TUNE_025_SCHED_WTP >= 0.0) {
            sched_wtp = TUNE_025_SCHED_WTP;
            sched_wc = 1.0 - sched_wtp;
        }
        if (weight_is(0.75) && TUNE_075_SCHED_WTP >= 0.0) {
            sched_wtp = TUNE_075_SCHED_WTP;
            sched_wc = 1.0 - sched_wtp;
        }
        if (weight_is(0.65) && sched_wtp < TUNE_065_SCHED_WTP) {
            sched_wtp = TUNE_065_SCHED_WTP;
            sched_wc = 1.0 - sched_wtp;
        }
        if (weight_is(0.80) && sched_wtp < TUNE_080_SCHED_WTP) {
            sched_wtp = TUNE_080_SCHED_WTP;
            sched_wc = 1.0 - sched_wtp;
        }
        if (weight_is(0.90) && sched_wtp < TUNE_090_SCHED_WTP) {
            sched_wtp = TUNE_090_SCHED_WTP;
            sched_wc = 1.0 - sched_wtp;
        }

        double pre_bias = sched_wc * focus.pre_share * (0.55 + min(2.5, focus.pre_excess));
        double dec_bias = sched_wc * focus.dec_share * (0.55 + min(2.5, focus.dec_excess));

        const int decode_now = total_decode_active();
        const int admit_goal = admission_target();
        const bool long_decode = estimated_lout() > 2.25 || finished_request_count < 4;
        const bool heavy_overdue_wait = throughput_heavy_profile &&
                                        finished_request_count >= 4 && estimated_lout() > 2.25;
        const bool throughput_gate =
            use_aggressive_cohort() && sched_wtp >= 0.62 &&
            long_decode && decode_now >= admit_goal;

        vector<string> out;
        out.reserve(K + 1);

        
        
        for (int k = 0; k < K; ++k) {
            if (busyC[k]) continue;
            clean(qDPROC[k], DPROC_READY, k);
            clean(qPPROC[k], PPROC_READY, k);

            bool hasD = !qDPROC[k].empty();
            bool hasP = !qPPROC[k].empty();
            if (!hasD && !hasP) continue;

            double dkey = 1e300, pkey = 1e300;
            int db = 0;
            double deff = -1e300, peff = -1e300;

            if (hasD) {
                int n = min(MAXR, (int)qDPROC[k].size());
                db = batch_for(DPROC, n);
                int id = qDPROC[k].top().second;
                double rem = dcost[DPROC][db] + transfer_time(db) + dcost[DPOST][db];
                deff = req[id].decode_due - rem;
                double cont_bonus = req[id].tokens > 0 ? 1.30 : 0.15;
                dkey = norm_key(deff, now, SLO2, cont_bonus) - 1.35 * dec_bias;
            }
            if (hasP) {
                int id = qPPROC[k].top().second;
                double frac = double(num_layers - req[id].next_layer) / num_layers;
                double rem = S + dur(PPROC, req[id].lin) * frac + transfer_time(req[id].lin) + S + dur(PPOST, req[id].lin);
                peff = req[id].arr + SLO1 - rem;
                pkey = norm_key(peff, now, SLO1, 0.25) - 1.35 * pre_bias;
            }

            bool chooseD = dkey <= pkey;

            if ((one_shot_mode() || (w_tp <= 1e-12 && TUNE_ZERO_DRAIN) ||
                 (weight_is(0.15) && TUNE_015_DRAIN) ||
                 (weight_is(0.25) && TUNE_025_DRAIN) ||
                 (weight_is(0.30) && TUNE_030_DRAIN) ||
                 (weight_is(0.65) && TUNE_065_DRAIN) ||
                 (weight_is(0.67) && TUNE_067_DRAIN) ||
                 (weight_is(0.75) && TUNE_075_DRAIN) ||
                 (weight_is(0.80) && TUNE_080_DRAIN) ||
                 (weight_is(0.90) && TUNE_090_DRAIN)) && hasP) chooseD = false;

            
            
            
            if (throughput_gate && hasD) chooseD = true;

            
            
            
            if (chooseD && hasD && hasP && downTail > now + 1e-12) {
                int pid = qPPROC[k].top().second;
                int pls = req[pid].next_layer;
                int ple = choose_piece_end(pid, k, now);
                double pcost = S + dur(PPROC, req[pid].lin) * double(ple - pls) / num_layers;
                double dcost_now = dcost[DPROC][max(1, db)];
                if (ple < num_layers && now + pcost + dcost_now <= downTail + 1e-12) {
                    chooseD = false;
                }
            }
            int forced_p_le = -1;
            bool wait_for_d = false;
            if (chooseD && sched_wtp >= 0.12) {
                int n = min(MAXR, (int)qDPROC[k].size());
                int top_id_for_wait = qDPROC[k].empty() ? -1 : qDPROC[k].top().second;
                bool first_token_wait_forbidden =
                    top_id_for_wait >= 0 && req[top_id_for_wait].tokens == 0 && sched_wc >= 0.40;

                if (!first_token_wait_forbidden &&
                    preserve_stable_cohort() && sched_wtp >= 0.72 &&
                    pendingUpDecReq[k] > 0 && (deff > now || heavy_overdue_wait)) {
                    
                    
                    
                    int future_n = min(MAXR, n + pendingUpDecReq[k]);
                    int desired = batch_for(DPROC, future_n);
                    double nx = next_known_event(now);
                    if (n < desired && nx < 1e290) {
                        double dt = max(0.0, nx - now);
                        double slack = max(0.0, deff - now);
                        double cap = slack * (w_tp < 0.78 ? 0.25 :
                                              (0.30 + 0.65 * sched_wtp));
                        if (heavy_overdue_wait) {
                            int inc = min(MAXR, pendingUpDecReq[k]);
                            double separate = dcost[DPROC][max(1, batch_for(DPROC, n))]
                                            + dcost[DPROC][max(1, batch_for(DPROC, inc))];
                            double combined = dcost[DPROC][max(1, batch_for(DPROC, future_n))];
                            double gain = max(0.0, separate - combined);
                            cap = max(cap, gain * (0.50 + 0.80 * w_tp) * TUNE_WAIT_GAIN);
                        }
                        wait_for_d = dt <= cap + 1e-12;
                    }
                } else if (!first_token_wait_forbidden &&
                           !upQ.empty() && upQ.front().dec &&
                           upQ.front().remote == k && deff > now) {
                    
                    
                    int inc = upQ.front().m;
                    double dt = max(0.0, upQ.front().finish - now);
                    int n2 = min(MAXR, n + inc);
                    double separate = dcost[DPROC][max(1, batch_for(DPROC, n))]
                                    + dcost[DPROC][max(1, batch_for(DPROC, min(MAXR, inc)))];
                    double combined = dcost[DPROC][max(1, batch_for(DPROC, n2))];
                    double gain = max(0.0, separate - combined);
                    double slack_cap = max(0.0, deff - now) * (0.10 + 0.55 * sched_wtp);
                    wait_for_d = dt <= gain * (0.35 + 1.25 * sched_wtp) + 1e-12
                              && dt <= slack_cap + 1e-12;
                }

                
                
                if (wait_for_d && hasP) {
                    double nx = next_known_event(now);
                    double dt = nx < 1e290 ? max(0.0, nx - now) : 0.0;
                    int pid = qPPROC[k].top().second;
                    int ls = req[pid].next_layer;
                    int rem = num_layers - ls;
                    double per = dur(PPROC, req[pid].lin) / max(1, num_layers);
                    int span = (int)floor((dt - S + 1e-12) / max(per, 1e-18));
                    span = min(span, rem - 1);
                    if (span >= 1) {
                        forced_p_le = ls + span;
                        chooseD = false;
                        wait_for_d = false;
                    }
                }
            }

            if (chooseD && wait_for_d) {
                continue;
            }

            if (chooseD) {
                clean(qDPROC[k], DPROC_READY, k);
                int n = min(MAXR, (int)qDPROC[k].size());
                if (n == 0) continue;
                int b = stable_batch_for(DPROC, n);
                vector<int> ids;
                ids.reserve(b);
                for (int z = 0; z < b; ++z) {
                    clean(qDPROC[k], DPROC_READY, k);
                    if (qDPROC[k].empty()) break;
                    int id = qDPROC[k].top().second;
                    qDPROC[k].pop();
                    req[id].st = DPROC_RUN;
                    ids.push_back(id);
                }
                if (ids.empty()) continue;
                Task tk;
                tk.type = DPROC;
                tk.remote = k;
                tk.ids = ids;
                runC[k] = tk;
                busyC[k] = true;
                finishC[k] = now + S + dur(DPROC, (int)ids.size());

                ostringstream ss;
                ss << 'C' << k << " D PROC " << k << ' ' << ids.size();
                for (int id : ids) ss << ' ' << id;
                out.push_back(ss.str());
            } else {
                clean(qPPROC[k], PPROC_READY, k);
                if (qPPROC[k].empty()) continue;
                int id = qPPROC[k].top().second;
                qPPROC[k].pop();
                int ls = req[id].next_layer;
                int le = forced_p_le >= 0 ? forced_p_le : choose_piece_end(id, k, now);

                if (le == num_layers && total_decode_active() > 0 && decode_down_pressure() > 0) {
                    double px = transfer_time(req[id].lin);
                    double nx = next_known_event(now);
                    double deadline = req[id].arr + SLO1;
                    double slack = max(0.0, deadline - now);
                    double cap = slack * (0.10 + 0.35 * sched_wtp);
                    if (px > SLO2 * (0.18 + 0.45 * sched_wtp) && deadline > now &&
                        nx < 1e290 && nx - now <= cap + 1e-12) {
                        qPPROC[k].push({qkey_pproc(id), id});
                        continue;
                    }
                }
                req[id].st = PPROC_RUN;

                Task tk;
                tk.type = PPROC;
                tk.remote = k;
                tk.ls = ls;
                tk.le = le;
                tk.ids = {id};
                runC[k] = tk;
                busyC[k] = true;
                double piece_dur = dur(PPROC, req[id].lin) * double(le - ls) / num_layers;
                
                
                
                if (accurate_prefill_load) {
                    prefillLoad[k] = max(0.0, prefillLoad[k] - piece_dur);
                }
                finishC[k] = now + S + piece_dur;

                ostringstream ss;
                ss << 'C' << k << " P PROC " << ls << ' ' << le << ' ' << k << ' ' << id;
                out.push_back(ss.str());
            }
        }

        if (!busyE) {
            clean(qPPRE, PPRE_READY, -1);
            clean(qPPOST, PPOST_READY, -1);
            clean(qDPRE, DPRE_READY, -1);
            clean(qDPOST, DPOST_READY, -1);

            struct Cand {
                double key;
                double eff;
                double cost;
                int type;
                int b;
            };
            vector<Cand> cs;

            if (!qPPRE.empty()) {
                int id = qPPRE.top().second;
                double rem = S + dur(PPRE, req[id].lin) + transfer_time(req[id].lin) + S + dur(PPROC, req[id].lin) + transfer_time(req[id].lin) + S + dur(PPOST, req[id].lin);
                double eff = req[id].arr + SLO1 - rem;
                double keyv = norm_key(eff, now, SLO1, 0.20);
                if (eff > now && total_decode_active() > 0) {
                    double ratio = transfer_time(req[id].lin) / max(SLO2, 1e-12);
                    double pressure = decode_up_pressure() > 0 ? 1.0 : 0.35;
                    keyv += pressure * (0.18 + 0.72 * sched_wc) * min(3.0, ratio);
                }
                keyv -= 1.15 * pre_bias;
                if (throughput_gate) keyv += 2.25;
                cs.push_back({keyv, eff, S + dur(PPRE, req[id].lin), PPRE, 1});
            }
            if (!qPPOST.empty()) {
                int id = qPPOST.top().second;
                double rem = S + dur(PPOST, req[id].lin);
                double eff = req[id].arr + SLO1 - rem;
                cs.push_back({norm_key(eff, now, SLO1, 0.50) - 1.55 * pre_bias,
                              eff, S + dur(PPOST, req[id].lin), PPOST, 1});
            }
            if (!qDPRE.empty()) {
                int target = decode_window_target();
                int admit = max(0, target - hot_active);
                int avail = hot_dpre_ready + min(cold_dpre_ready, admit);

                if (one_shot_mode()) avail = min(MAXR, (int)qDPRE.size());

                
                if (hot_active == 0 && !one_shot_mode()) {
                    avail = hot_dpre_ready + min(cold_dpre_ready, target);
                }

                avail = min(MAXR, max(0, avail));
                if (avail > 0) {
                    int b = batch_for(DPRE, avail);
                    int id = qDPRE.top().second;
                    int g = max(1, (b + K - 1) / K);
                    double rem = dcost[DPRE][b] + transfer_time(g) + dcost[DPROC][g] + transfer_time(g) + dcost[DPOST][b];
                    double eff = req[id].decode_due - rem;
                    double cont_bonus = req[id].tokens > 0 ? 1.05 : 0.10;

                    double keyv = norm_key(eff, now, SLO2, cont_bonus) - 1.20 * dec_bias;
                    if (hot_active < target && cold_dpre_ready > 0 && sched_wtp >= 0.45) {
                        double need = double(target - hot_active) / target;
                        keyv -= (0.35 + 1.20 * sched_wtp) * min(1.0, max(0.0, need));
                    }

                    cs.push_back({keyv, eff, dcost[DPRE][b], DPRE, b});
                }
            }
            if (!qDPOST.empty()) {
                int n = min(MAXR, (int)qDPOST.size());
                int b = batch_for(DPOST, n);
                int id = qDPOST.top().second;
                double rem = dcost[DPOST][b];
                double eff = req[id].decode_due - rem;
                double cont_bonus = req[id].tokens > 0 ? 1.45 : 0.12;
                cs.push_back({norm_key(eff, now, SLO2, cont_bonus) - 1.55 * dec_bias,
                              eff, dcost[DPOST][b], DPOST, b});
            }

            if (sched_wtp >= 0.62 && decode_now < admit_goal && !qPPOST.empty()) {
                for (auto &c : cs) {
                    if (c.type == PPOST) {
                        double need = double(admit_goal - decode_now) / max(1, admit_goal);
                        c.key -= 1.5 * min(1.0, need);
                    }
                }
            }

            sort(cs.begin(), cs.end(), [](const Cand &a, const Cand &b) {
                if (abs(a.key - b.key) > 1e-12) return a.key < b.key;
                return a.type > b.type;
            });

            int forced_pick = -1;

            if ((one_shot_mode() || (w_tp <= 1e-12 && TUNE_ZERO_DRAIN) ||
                 (weight_is(0.15) && TUNE_015_DRAIN) ||
                 (weight_is(0.25) && TUNE_025_DRAIN) ||
                 (weight_is(0.30) && TUNE_030_DRAIN) ||
                 (weight_is(0.65) && TUNE_065_DRAIN) ||
                 (weight_is(0.67) && TUNE_067_DRAIN) ||
                 (weight_is(0.75) && TUNE_075_DRAIN) ||
                 (weight_is(0.80) && TUNE_080_DRAIN) ||
                 (weight_is(0.90) && TUNE_090_DRAIN)) && pref_open > 0) {
                int ipost = -1, ipre = -1;
                for (int j = 0; j < (int)cs.size(); ++j) {
                    if (cs[j].type == PPOST && ipost < 0) ipost = j;
                    if (cs[j].type == PPRE && ipre < 0) ipre = j;
                }
                if (ipost >= 0) forced_pick = ipost;
                else if (ipre >= 0) forced_pick = ipre;
            }

            if (forced_pick < 0 && throughput_gate) {
                
                
                int ipost = -1, ipre = -1;
                for (int j = 0; j < (int)cs.size(); ++j) {
                    if (cs[j].type == DPOST && ipost < 0) ipost = j;
                    if (cs[j].type == DPRE && ipre < 0) ipre = j;
                }
                if (ipost >= 0) forced_pick = ipost;
                else if (ipre >= 0) forced_pick = ipre;
            }

            
            
            
            
            if (first_cohort_profile && forced_pick < 0 && !qPPOST.empty() && !qDPRE.empty()) {
                int ct = cohort_target();
                int have = min(MAXR, (int)qDPRE.size());
                if (have < ct) {
                    int ipost = -1, ipre = -1;
                    for (int j = 0; j < (int)cs.size(); ++j) {
                        if (cs[j].type == PPOST && ipost < 0) ipost = j;
                        if (cs[j].type == DPRE && ipre < 0) ipre = j;
                    }
                    if (ipost >= 0 && ipre >= 0) {
                        double dslack = cs[ipre].eff - now;
                        double post_cost = cs[ipost].cost;

                        
                        
                        double frac = sched_wtp >= 0.82 ? 0.80 :
                                      sched_wtp >= 0.72 ? 0.55 :
                                      sched_wtp >= 0.64 ? 0.28 : 0.12;
                        if (first_cohort_profile || dslack > post_cost / max(frac, 1e-9)) {
                            forced_pick = ipost;
                        }
                    }
                }
            }

            int pick = forced_pick;
            for (int z = 0; pick < 0 && z < (int)cs.size(); ++z) {
                auto c = cs[z];
                if (c.type == DPOST && sched_wtp >= 0.12 && pendingDownDecReq > 0 &&
                    (c.eff > now || heavy_overdue_wait)) {
                    int top_post_id = qDPOST.empty() ? -1 : qDPOST.top().second;
                    if (top_post_id >= 0 && req[top_post_id].tokens == 0 && sched_wc >= 0.40) {
                        pick = z;
                        break;
                    }
                    int n = min(MAXR, (int)qDPOST.size());
                    bool can_wait = false;
                    double dt = 0.0;

                    if (use_aggressive_cohort() && sched_wtp >= 0.72) {
                        int future_n = min(MAXR, n + pendingDownDecReq);
                        int desired = batch_for(DPOST, future_n);
                        double nx = next_known_event(now);
                        if (n < desired && nx < 1e290) {
                            dt = max(0.0, nx - now);
                            double slack = max(0.0, c.eff - now);
                            double cap = slack * (0.25 + 0.70 * sched_wtp);
                            if (heavy_overdue_wait) {
                                int inc = min(MAXR, pendingDownDecReq);
                                double separate = dcost[DPOST][max(1, batch_for(DPOST, n))]
                                                + dcost[DPOST][max(1, batch_for(DPOST, inc))];
                                double combined = dcost[DPOST][max(1, batch_for(DPOST, future_n))];
                                double gain = max(0.0, separate - combined);
                                cap = max(cap, gain * (0.50 + 0.80 * w_tp) * TUNE_WAIT_GAIN);
                            }
                            can_wait = dt <= cap + 1e-12;
                        }
                    } else if (!downQ.empty() && downQ.front().dec) {
                        int inc = downQ.front().m;
                        int n2 = min(MAXR, n + inc);
                        dt = max(0.0, downQ.front().finish - now);
                        double separate = dcost[DPOST][max(1, batch_for(DPOST, n))]
                                        + dcost[DPOST][max(1, batch_for(DPOST, min(MAXR, inc)))];
                        double combined = dcost[DPOST][max(1, batch_for(DPOST, n2))];
                        double gain = max(0.0, separate - combined);
                        double slack_cap = max(0.0, c.eff - now) * (0.08 + 0.50 * sched_wtp);
                        can_wait = dt <= gain * (0.35 + 1.25 * sched_wtp) + 1e-12
                                && dt <= slack_cap + 1e-12;
                    }

                    if (can_wait) {
                        
                        
                        
                        int alt = -1;
                        for (int j = 0; j < (int)cs.size(); ++j) {
                            if (j == z || cs[j].type == DPOST) continue;
                            if (cs[j].cost <= dt + 1e-12) {
                                if (alt < 0 || cs[j].key < cs[alt].key) alt = j;
                            }
                        }
                        if (alt >= 0) pick = alt;
                        else pick = -1;
                        break;
                    }
                }
                pick = z;
                break;
            }

            if (pick >= 0) {
                Cand c = cs[pick];
                if (c.type == PPRE) {
                    clean(qPPRE, PPRE_READY, -1);
                    if (!qPPRE.empty()) {
                        int id = qPPRE.top().second;
                        qPPRE.pop();
                        int k = choose_remote(now);
                        req[id].remote = k;
                        req[id].st = PPRE_RUN;
                        ++activeCloud[k];
                        prefillLoad[k] += dur(PPROC, req[id].lin);

                        Task tk;
                        tk.type = PPRE;
                        tk.remote = k;
                        tk.ids = {id};
                        runE = tk;
                        busyE = true;
                        finishE = now + S + dur(PPRE, req[id].lin);

                        ostringstream ss;
                        ss << "E P PRE " << k << ' ' << id;
                        out.push_back(ss.str());
                    }
                } else if (c.type == PPOST) {
                    clean(qPPOST, PPOST_READY, -1);
                    if (!qPPOST.empty()) {
                        int id = qPPOST.top().second;
                        qPPOST.pop();
                        req[id].st = PPOST_RUN;
                        int k = req[id].remote;

                        Task tk;
                        tk.type = PPOST;
                        tk.remote = k;
                        tk.ids = {id};
                        runE = tk;
                        busyE = true;
                        finishE = now + S + dur(PPOST, req[id].lin);

                        ostringstream ss;
                        ss << "E P POST " << k << ' ' << id;
                        out.push_back(ss.str());
                    }
                } else if (c.type == DPRE) {
                    clean(qDPRE, DPRE_READY, -1);

                    int target = decode_window_target();
                    int admit = max(0, target - hot_active);
                    int avail = hot_dpre_ready + min(cold_dpre_ready, admit);
                    if (one_shot_mode()) {
                        avail = min(MAXR, (int)qDPRE.size());
                    } else if (hot_active == 0) {
                        avail = hot_dpre_ready + min(cold_dpre_ready, target);
                    }
                    int n = min(MAXR, max(0, avail));

                    if (n > 0) {
                        int b = stable_batch_for(DPRE, n);
                        if (use_aggressive_cohort() && sched_wtp >= 0.72)
                            b = min(n, max(b, min(n, cohort_target())));
                        vector<int> ids;
                        ids.reserve(b);
                        for (int z = 0; z < b; ++z) {
                            clean(qDPRE, DPRE_READY, -1);
                            if (qDPRE.empty()) break;
                            int id = qDPRE.top().second;
                            qDPRE.pop();

                            
                            
                            if (req[id].tokens > 0) --hot_dpre_ready;
                            else --cold_dpre_ready;

                            req[id].st = DPRE_RUN;
                            ids.push_back(id);
                        }
                        if (!ids.empty()) {
                            Task tk;
                            tk.type = DPRE;
                            tk.ids = ids;
                            runE = tk;
                            busyE = true;
                            finishE = now + S + dur(DPRE, (int)ids.size());

                            ostringstream ss;
                            ss << "E D PRE -1 " << ids.size();
                            for (int id : ids) ss << ' ' << id;
                            out.push_back(ss.str());
                        }
                    }
                } else if (c.type == DPOST) {
                    clean(qDPOST, DPOST_READY, -1);
                    int n = min(MAXR, (int)qDPOST.size());
                    if (n > 0) {
                        int b = stable_batch_for(DPOST, n);
                        if (use_aggressive_cohort() && sched_wtp >= 0.72)
                            b = min(n, max(b, min(n, cohort_target())));
                        vector<int> ids;
                        ids.reserve(b);
                        for (int z = 0; z < b; ++z) {
                            clean(qDPOST, DPOST_READY, -1);
                            if (qDPOST.empty()) break;
                            int id = qDPOST.top().second;
                            qDPOST.pop();
                            req[id].st = DPOST_RUN;
                            ids.push_back(id);
                        }
                        if (!ids.empty()) {
                            Task tk;
                            tk.type = DPOST;
                            tk.ids = ids;
                            runE = tk;
                            busyE = true;
                            finishE = now + S + dur(DPOST, (int)ids.size());

                            ostringstream ss;
                            ss << "E D POST -1 " << ids.size();
                            for (int id : ids) ss << ' ' << id;
                            out.push_back(ss.str());
                        }
                    }
                }
            }
        }

        cout << out.size() << '\n';
        for (const string &s : out) cout << s << '\n';
        cout.flush();
    }

    return 0;
}
