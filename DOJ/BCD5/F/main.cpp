#include "library/template.hpp"
#include "library/number/modint.hpp"
#include "library/math/factorial.hpp"

using namespace std; 
using namespace suisen; 
using mint = modint1000000007; 
using ull = unsigned long long; 
using ll = long long; 
constexpr int V = 450; 
constexpr int B = 45; 
struct Query{
    int l, r, x, y; 
}; 

signed main() {
    ios::sync_with_stdio(false); 
    cin.tie(NULL); 
    static unsigned char rem[B + 1][V + 1]; 
    REP(x, 1, B + 1) {
        REP(v, V + 1) {
            rem[x][v] = v % x; 
        }
    }
    factorial<mint> fac(200000); 
    int T; 
    read(T); 
    while (T--) {
        int N, Q;
        read(N, Q); 
        vector<int> A(N + 1); 
        REP(i, 1, N + 1) {
            read(A[i]); 
        }   
        vector<Query> qs(Q); 
        vector<vector<int>> g(B + 1); 
        REP(i, Q) {
            auto& [l, r, x, y] = qs[i]; 
            read(l, r, x, y); 
            if (x <= B) {
                g[x].push_back(i); 
            }
        }
        int nb = (N + 63) >> 6; 
        vector<ull> bit(( V + 1) * nb); 
        vector<int> pref((V + 1) * (nb + 1)); 
        REP(i, 1, N + 1) {
            int v = A[i]; 
            int b = (i -1 ) >> 6; 
            int k = (i - 1) & 63; 
            bit[v * nb + b] |= 1ULL << k; 
        }
        REP(v, V + 1) {
            int bp = v * nb; 
            int pp = v * (nb + 1); 
            REP(b, nb) {
                pref[pp + b + 1] = pref[pp + b] + __builtin_popcountll(bit[bp + b]); 
            }
        }
        auto rank = [&](int v, int p) {
            int b = p >> 6; 
            int k = p & 63; 
            int res = pref[v * (nb + 1) + b]; 
            if (k) {
                ull mask = (1ULL << k) - 1; 
                res += __builtin_popcountll(bit[v * nb + b] & mask); 
            }
            return res; 
        }; 
        auto range_count = [&](int v, int l, int r) {
            return rank(v, r) - rank(v, l -1); 
        }; 
        vector<int> ans(Q); 
        REP(id, Q) {
            auto[l, r, x, y] = qs[id]; 
            if (x <= B) {
                continue; 
            }
            for (int v = y; v <= V; v += x) {
                ans[id] +=range_count(v,l, r); 

            }
        }
        REP(x, 1, B + 1) {
            if (g[x].empty()) {
                continue; 
            }
            int m = g[x].size(); 
            vector<int> head(N + 1, -1); 
            vector<int> nxt(2 * m); 
            vector<int> id(2 * m); 
            vector<signed char> sgn(2 * m);
            int ptr = 0; 
            auto add = [&](int p, int qi, int s) {
                id[ptr] = qi; 
                sgn[ptr] = s; 
                nxt[ptr] = head[p]; 
                head[p] = ptr++; 
            } ;
            for (int qi : g[x]) {
                add(qs[qi].r, qi, +1); 
                add(qs[qi].l - 1, qi, -1); 
            }
            vector<int> cnt(x); 
            REP(p, N + 1) {
                if (p) {
                    ++cnt[rem[x][A[p]]]; 
                }
                for (int e = head[p]; e != -1; e = nxt[e]) {
                    int qi = id[e]; 
                    ans[qi] += sgn[e] * cnt[qs[qi].y]; 
                }
            }
        }
        REP(i, Q) {
            print(ans[i]); 
        }
    }
}