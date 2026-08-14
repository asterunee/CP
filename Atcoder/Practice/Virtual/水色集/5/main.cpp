#include "library/template.hpp"

using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int N,M;
    read(N, M); 
    vector<vector<int>> byL(N + 2), byR(N + 2); 
    vector<int> mn(N + 2, N + 1); 
    unordered_map<ull, int> cnt; 
    cnt.reserve(2 * M); 
    cnt.max_load_factor(0.7); 
    auto key = [&](int l, int r) -> ull {return (ull(l) << 32) | ull(r); };
    REP(_, M) {
        int L, R; 
        read(L, R); 
        byL[L].push_back(R);
        byR[R].push_back(L);
        chmin(mn[L], R); 
        ++cnt[key(L, R)];  
    }
    REP(i, 1, N + 1) {
        sort(ALL(byL[i])); 
        sort(ALL(byR[i])); 
    }
    RREP(i, 1, N + 1) {
        chmin(mn[i], mn[i + 1]); 
    }
    int tt; 
    read(tt); 
    while (tt--) {
        int S, T;
        read(S, T); 
        int c = 0; 
        if (auto it = cnt.find(key(S, T)); it != cnt.end()) {
            c = it -> second; 
        }
        if (c) {
            bool ok = false; 
            if (c >= 2) {
                ok = true; 
            }
            if (S + 1 <= N && mn[S + 1] <= T) {
                ok =true;  
            }
            if (mn[S] <= T - 1) {
                ok = true; 
            }
            print(ok ? "Yes" : "No"); 
            continue; 
        }
        bool ok = false; 
        auto & vr = byL[S]; 
        auto itR = upper_bound(ALL(vr), T); 
        auto & vl = byR[T]; 
        auto itL = lower_bound(ALL(vl), S);
        if (itR != vr.begin() && itL != vl.end()) {
            --itR; 
            int R1 = *itR; 
            int L2 = *itL;
            if (L2 <= R1 + 1) {
                ok = true; 
            } 
        } 
        print(ok ? "Yes" : "No"); 
    }
}