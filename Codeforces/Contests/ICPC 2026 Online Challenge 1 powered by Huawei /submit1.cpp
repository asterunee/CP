#include <bits/stdc++.h>
using namespace std;

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
    State st = NONE;
    double arr = 0;
    double decode_due = 0;
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

    const int MAXR = 2000;
    vector<double> dcost[6];
    vector<int> bestB[6];
    for (int t = DPRE; t <= DPOST; ++t) {
        dcost[t].assign(MAXR + 1, 0);
        bestB[t].assign(MAXR + 1, 1);
        for (int b = 1; b <= MAXR; ++b) dcost[t][b] = S + dur(t, b);

        for (int n = 1; n <= MAXR; ++n) {
            double best_make = 1e300, best_mean = 1e300;
            for (int b = 1; b <= n; ++b) {
                int q = n / b, r = n % b;
                double make = q * dcost[t][b] + (r ? dcost[t][r] : 0.0);
                double sum = dcost[t][b] * (double)b * q * (q + 1) / 2.0;
                if (r) sum += (double)r * (q * dcost[t][b] + dcost[t][r]);
                double mean = sum / n;
                best_make = min(best_make, make);
                best_mean = min(best_mean, mean);
            }
            double best_u = -1;
            int choose = 1;
            for (int b = 1; b <= n; ++b) {
                int q = n / b, r = n % b;
                double make = q * dcost[t][b] + (r ? dcost[t][r] : 0.0);
                double sum = dcost[t][b] * (double)b * q * (q + 1) / 2.0;
                if (r) sum += (double)r * (q * dcost[t][b] + dcost[t][r]);
                double mean = sum / n;
                double u = w_tp * (best_make / make) + w_c * (best_mean / mean);
                if (u > best_u + 1e-12 || (abs(u - best_u) <= 1e-12 && ((w_tp >= w_c && b > choose) || (w_tp < w_c && b < choose)))) {
                    best_u = u;
                    choose = b;
                }
            }
            bestB[t][n] = choose;
        }
    }

    vector<Req> req(MAXR + 5);
    MinQ qPPRE, qPPOST, qDPRE, qDPOST;
    vector<MinQ> qPPROC(K), qDPROC(K);

    bool busyE = false;
    vector<char> busyC(K, false);
    Task runE;
    vector<Task> runC(K);
    double finishE = 0;
    vector<double> finishC(K, 0);

    vector<int> activeCloud(K, 0), decodeActive(K, 0);
    vector<double> prefillLoad(K, 0);
    vector<int> pendingUpDecReq(K, 0);
    int pendingDownDecReq = 0;

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
        for (int k = 0; k < K; ++k) {
            double rem_busy = busyC[k] ? max(0.0, finishC[k] - now) : 0.0;
            double score = rem_busy + prefillLoad[k] + decodeActive[k] * base_dec_cycle + pendingUpDecReq[k] * dcost[DPROC][1] * 0.5;
            score += activeCloud[k] * dcost[DPROC][1] * 0.15;
            if (score < best_score - 1e-12) {
                best_score = score;
                best = k;
            }
        }
        return best;
    };

    auto choose_piece_end = [&](int id, int k) {
        int ls = req[id].next_layer;
        int rem = num_layers - ls;
        if (rem <= 1) return num_layers;
        if (w_c < 0.08 || decodeActive[k] == 0) return num_layers;

        double full = dur(PPROC, req[id].lin);
        double per_layer = full / num_layers;
        double quantum = max(3.0 * S, base_dec_cycle * (0.75 + 3.25 * w_tp));
        double slo_quantum = max(3.0 * S, SLO2 * (0.20 + 1.30 * w_tp));
        quantum = min(quantum, slo_quantum);
        double comp_budget = max(per_layer, quantum - S);
        int span = max(1, (int) floor(comp_budget / per_layer + 1e-12));
        span = min(span, rem);
        return ls + span;
    };

    auto norm_key = [&](double effective_start, double now, double target, double bonus) {
        double slack = (effective_start - now) / max(target, 1e-9);
        double slack_weight = 0.35 + 0.65 * w_c;
        return slack_weight * slack - 2.0 * w_tp * bonus;
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
                req[id].st = PPRE_READY;
                qPPRE.push({now + SLO1, id});
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
                }

                for (int id : ids) {
                    if (!dec) {
                        if (dir == "UP") {
                            req[id].st = PPROC_READY;
                            qPPROC[remote].push({req[id].arr + SLO1, id});
                        } else {
                            req[id].st = PPOST_READY;
                            qPPOST.push({req[id].arr + SLO1, id});
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
                        qPPROC[tk.remote].push({req[id].arr + SLO1, id});
                    }
                } else if (tk.type == PPOST) {
                    int id = tk.ids[0];
                    int k = req[id].remote;
                    prefillLoad[k] -= dur(PPROC, req[id].lin);
                    if (prefillLoad[k] < 0 && prefillLoad[k] > -1e-8) prefillLoad[k] = 0;
                    ++decodeActive[k];
                    req[id].decode_due = now + SLO2;
                    req[id].st = DPRE_READY;
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
                    for (int id : tk.ids) {
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
                req[id].st = FINISHED;
                if (k >= 0) {
                    --activeCloud[k];
                    --decodeActive[k];
                }
            }
        }
        for (int id : post_done) {
            if (req[id].st == DPOST_DONE) {
                req[id].st = DPRE_READY;
                qDPRE.push({req[id].decode_due, id});
            }
        }

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
                db = bestB[DPROC][n];
                int id = qDPROC[k].top().second;
                double rem = dcost[DPROC][db] + transfer_time(db) + dcost[DPOST][db];
                deff = req[id].decode_due - rem;
                dkey = norm_key(deff, now, SLO2, 1.0);
            }
            if (hasP) {
                int id = qPPROC[k].top().second;
                double frac = double(num_layers - req[id].next_layer) / num_layers;
                double rem = S + dur(PPROC, req[id].lin) * frac + transfer_time(req[id].lin) + S + dur(PPOST, req[id].lin);
                peff = req[id].arr + SLO1 - rem;
                pkey = norm_key(peff, now, SLO1, 0.25);
            }

            bool chooseD = dkey <= pkey;
            if (chooseD) {
                int n = min(MAXR, (int)qDPROC[k].size());
                int future_n = min(MAXR, n + max(0, pendingUpDecReq[k]));
                int desired = bestB[DPROC][future_n];
                double nx = next_known_event(now);
                bool can_wait = pendingUpDecReq[k] > 0 && n < desired && nx < 1e290 && deff > now;
                if (can_wait) {
                    double allowed = w_tp * max(0.0, deff - now);
                    can_wait = nx - now <= allowed + 1e-12;
                }
                if (can_wait && !(hasP && peff <= now)) {
                    continue;
                }
                if (can_wait && hasP && peff <= now) chooseD = false;
            }

            if (chooseD) {
                clean(qDPROC[k], DPROC_READY, k);
                int n = min(MAXR, (int)qDPROC[k].size());
                if (n == 0) continue;
                int b = bestB[DPROC][n];
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
                int le = choose_piece_end(id, k);
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
                int type;
                int b;
            };
            vector<Cand> cs;

            if (!qPPRE.empty()) {
                int id = qPPRE.top().second;
                double rem = S + dur(PPRE, req[id].lin) + transfer_time(req[id].lin) + S + dur(PPROC, req[id].lin) + transfer_time(req[id].lin) + S + dur(PPOST, req[id].lin);
                double eff = req[id].arr + SLO1 - rem;
                cs.push_back({norm_key(eff, now, SLO1, 0.20), eff, PPRE, 1});
            }
            if (!qPPOST.empty()) {
                int id = qPPOST.top().second;
                double rem = S + dur(PPOST, req[id].lin);
                double eff = req[id].arr + SLO1 - rem;
                cs.push_back({norm_key(eff, now, SLO1, 0.50), eff, PPOST, 1});
            }
            if (!qDPRE.empty()) {
                int n = min(MAXR, (int)qDPRE.size());
                int b = bestB[DPRE][n];
                int id = qDPRE.top().second;
                int g = max(1, (b + K - 1) / K);
                double rem = dcost[DPRE][b] + transfer_time(g) + dcost[DPROC][g] + transfer_time(g) + dcost[DPOST][b];
                double eff = req[id].decode_due - rem;
                cs.push_back({norm_key(eff, now, SLO2, 0.80), eff, DPRE, b});
            }
            if (!qDPOST.empty()) {
                int n = min(MAXR, (int)qDPOST.size());
                int b = bestB[DPOST][n];
                int id = qDPOST.top().second;
                double rem = dcost[DPOST][b];
                double eff = req[id].decode_due - rem;
                cs.push_back({norm_key(eff, now, SLO2, 1.00), eff, DPOST, b});
            }

            sort(cs.begin(), cs.end(), [](const Cand &a, const Cand &b) {
                if (abs(a.key - b.key) > 1e-12) return a.key < b.key;
                return a.type > b.type;
            });

            int pick = -1;
            for (int z = 0; z < (int)cs.size(); ++z) {
                auto c = cs[z];
                if (c.type == DPOST) {
                    int n = min(MAXR, (int)qDPOST.size());
                    int future_n = min(MAXR, n + max(0, pendingDownDecReq));
                    int desired = bestB[DPOST][future_n];
                    double nx = next_known_event(now);
                    bool can_wait = pendingDownDecReq > 0 && n < desired && nx < 1e290 && c.eff > now;
                    if (can_wait) {
                        double allowed = w_tp * max(0.0, c.eff - now);
                        can_wait = nx - now <= allowed + 1e-12;
                    }
                    if (can_wait) {
                        bool urgent_other = false;
                        for (int j = z + 1; j < (int)cs.size(); ++j) if (cs[j].eff <= now) {
                            pick = j;
                            urgent_other = true;
                            break;
                        }
                        if (!urgent_other) pick = -1;
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
                    int n = min(MAXR, (int)qDPRE.size());
                    if (n > 0) {
                        int b = bestB[DPRE][n];
                        vector<int> ids;
                        ids.reserve(b);
                        for (int z = 0; z < b; ++z) {
                            clean(qDPRE, DPRE_READY, -1);
                            if (qDPRE.empty()) break;
                            int id = qDPRE.top().second;
                            qDPRE.pop();
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
                        int b = bestB[DPOST][n];
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
