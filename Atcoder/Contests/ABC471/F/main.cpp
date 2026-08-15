#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int N, K;
    read(N, K); 
    vector<string> S(N), T(N); 
    read(S); 
    vector<int> len(N), z(N); 
    bool any = false; 
    REP(i, N) {
        len[i] = S[i].size(); 
        while (z[i] < len[i] && S[i][z[i]] == '0') {
            ++z[i]; 
        }
        if (z[i] < len[i]) {
            T[i] = S[i].substr(z[i]); 
            any = true; 
        }
    }
    if (not any) {
        print(0); 
        return 0; 
    }
    vector<int> p(N); 
    iota(ALL(p), 0); 
    sort(ALL(p), [&](int i, int j) {if (len[i] != len[j]){return len[i] > len[j]; } if (S[i] != S[j]) {return S[i] > S[j]; }return i < j; });
    vector<unsigned char> in(N); 
    REP(i, K - 1) {
        in[p[i]] = true; 
    }
    int q = p[K - 1]; 
    vector<int> ord(p.begin(), p.begin() + K); 
        auto cmp = [&](int i, int j) {
        const string& a = S[i];
        const string& b = S[j];
        int n = a.size();
        int m = b.size();
        REP(k, n + m) {
            char x = k < n ? a[k] : b[k - n];
            char y = k < m ? b[k] : a[k - m];
            if (x != y) {
                return x > y;
            }
        }
        return i < j;
    };
    sort(ALL(ord), cmp);
    vector<int> L(N, -1), R(N, -1);
    string V;
    V.reserve(10 * K);
    for (int i : ord) {
        L[i] = V.size();
        V += S[i];
        R[i] = V.size();
    }
    int M = V.size();
    constexpr ull B1 = 911382323ULL;
    constexpr ull B2 = 972663749ULL;
    vector<ull> pw1(M + 12, 1), pw2(M + 12, 1);
    vector<ull> hv1(M + 1), hv2(M + 1);
    REP(i, M + 11) {
        pw1[i + 1] = pw1[i] * B1;
        pw2[i + 1] = pw2[i] * B2;
    }

    REP(i, M) {
        ull x = V[i] - '0' + 1;

        hv1[i + 1] = hv1[i] * B1 + x;
        hv2[i + 1] = hv2[i] * B2 + x;
    }
    vector<array<ull, 11>> ht1(N), ht2(N);
    REP(i, N) {
        REP(j, (int) T[i].size()) {
            ull x = T[i][j] - '0' + 1;

            ht1[i][j + 1] = ht1[i][j] * B1 + x;
            ht2[i][j + 1] = ht2[i][j] * B2 + x;
        }
    }

    auto sub = [&](int l, int r) {
        return pair<ull, ull>{
            hv1[r] - hv1[l] * pw1[r - l],
            hv2[r] - hv2[l] * pw2[r - l]
        };
    };
    auto suffix_hash = [&](int d, int x) {
        int l = L[d];
        int r = R[d];
        if (x <= l) {
            return sub(0, x);
        }
        int rem = x - l;
        auto a = sub(0, l);
        auto b = sub(r, r + rem);
        return pair<ull, ull>{
            a.first * pw1[rem] + b.first,
            a.second * pw2[rem] + b.second
        };
    };
    auto hash_prefix = [&](int i, int d, int m) {
        int t = T[i].size();
        if (m <= t) {
            return pair<ull, ull> {
                ht1[i][m],
                ht2[i][m]
            };
        }
        int x = m - t;
        auto h = suffix_hash(d, x);
        return pair<ull, ull> {
            ht1[i][t] * pw1[x] + h.first,
            ht2[i][t] * pw2[x] + h.second
        };
    };
    auto get_char = [&](int i, int d, int k) {
        int t = T[i].size();
        if (k < t) {
            return T[i][k];
        }
        int x = k - t;
        if (x < L[d]) {
            return V[x];
        }
        return V[R[d] + x - L[d]];
    };

    int best = -1;
    int best_len = -1;
    REP(i, N) {
        if (T[i].empty()) {
            continue;
        }
        int d = in[i] ? i : q;
        int cur_len = (int) T[i].size()+ M- (R[d] - L[d]);
        if (cur_len > best_len) {
            best_len = cur_len;
            best = i;
            continue;
        }
        if (cur_len < best_len) {
            continue;
        }
        int bd = in[best] ? best : q;
        int lo = 0;
        int hi = best_len;
        while (lo < hi) {
            int md = (lo + hi + 1) / 2;
            if (hash_prefix(i, d, md) == hash_prefix(best, bd, md)) {
                lo = md;
            } else {
                hi = md - 1;
            }
        }
        if (lo < best_len && get_char(i, d, lo) > get_char(best, bd, lo)) {
            best = i;
        }
    }
    int d = in[best] ? best : q;
    string ans;
    ans.reserve(best_len);
    ans += T[best];
    ans.append(V, 0, L[d]);
    ans.append(V, R[d], string::npos);
    print(ans);
}