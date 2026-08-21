#include "library/template.hpp"

using namespace std;
using namespace suisen;
using ll = long long;
struct Run{int l, r, len; }; 
signed main() {
    
        int N; 
        read(N); 
        string S; 
        read(S); 
        vector<Run> run; 
        for (int i = 0; i < N; ) {
            if (S[i] == '0') {
                ++i; 
                continue;
            }
            int l = i + 1; 
            while (i < N and S[i] == '1') {
                ++i; 
            }
            int r = i; 
            run.push_back({l, r, r - l + 1}); 
        }
        if (run.empty()) {
            print(0); 
            return 0; 
        }
        int M = run.size(); 
        int mx = 0; 
        for (auto [l, r, len] : run) {
            chmax(mx, len); 
        }
        vector<vector<int>> del(mx + 1); 
        REP(i, M) {
            del[run[i].len].push_back(i); 
        }
        vector<int> pre(M), nxt(M); 
        REP(i, M) {
            pre[i] = i- 1; 
            nxt[i] = i + 1 < M ? i + 1 : -1; 

        }
        auto tail = [&](ll e, ll r) -> ll {int cnt = r  -e; if (cnt <= 0) {return 0; } return cnt * ((N - e) + (N - r + 1)) / 2;};
        auto calc = [&](int i, int p, int k) -> ll{ll l = run[i].l, r = run[i].r, e = l + k - 1, gap = p == -1 ? l : e - run[p].r; return gap * (N - e+1) + tail(e, r);  };  
        auto diff = [&](int i, int p, int k) -> ll{if (p == -1 ){ return N - k + 1; } return run[i].l + k - 1 - run[p].r;};
        ll cur = 0,d = 0; 
        REP(i, M) {
            cur += calc(i, i - 1, 1); 
            d += diff(i, i - 1, 1) ;
        }
        ll ans = 0 , cnt = M; 
        REP(k, 1, mx + 1) {
            ans += cur; 
            for (int x : del[k]) {
                int p = pre[x], q = nxt[x]; 
                cur -= calc(x, p, k); 
                d -= diff(x, p, k); 
                if (q != -1) {
                    cur -= calc(q, x, k); 
                    d -= diff(q, x, k); 
                    cur += calc(q, p, k); 
                    d += diff(q, p, k); 
                    pre[q] = p; 
                }
                if (p != -1) {
                    nxt[p] = q; 
                }
                --cnt; 
            }
            if (cnt) {
                cur -=d; 
                d += cnt - 2; 
            }
        }

        print(ans); 
}