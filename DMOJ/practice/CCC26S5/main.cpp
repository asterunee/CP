#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
namespace {
    constexpr ll INF = 1LL << 62; 
    void add(vector<ll>& a, ll x) {
        REP(d, -10, 11) {
            a.push_back(x +d); 
        }
    }
    void add_frac(vector<ll>& a, ll x, ll y) {
        add(a, x / y); 
    }
    ll rect (ll n, ll m, ll s) {
        if (n <= 0 || m <= 0 || s < 2) {
            return 0; 
        }
        ll t = min(s, n + m); 
        ll l = max(1LL, t - m); 
        ll r = min(n, t - 1); 
        if (l > r) {
            return 0; 
        }

        ll ans = 0; 
        for (ll x : {l, r, t / 2, (t + 1) / 2}) {
            x = min(r, max(l, x)); 
            chmax(ans, x * (t - x)); 
        }
        return ans; 
    }
    ll calc(ll A, ll B, ll U, ll V, ll C) {
        if (U <= 0 || V <= 0 || C < 4) {
            return -INF; 
        }
        ll ans = -INF; 
        auto eval = [&](ll u, ll v) {if (u < 1 || u > U || v < 1 || v > V || u > v){return; }ll rem = C - 2 * u - 2 * v; if (rem < 0) {return; }ll q = min(u, rem); chmax(ans, A * B + A * v + B * u + q * (v - 1)); };
        ll umax = min({U, V, C / 5}); 
        if (umax >= 1) {
            vector<ll> cand; 
            add(cand, 1); 
            add(cand, umax); 
            add_frac(cand, C - 2 * V, 3); 
            REP(r, 2) {
                add_frac(cand, -3 * A + 2 * B + C - r - 2, 6); 
            }
            for (ll u : cand) {
                if (u < 1 || u > umax) {
                    continue; 
                }
                ll v = min(V, (C - 3 * u) / 2); 
                if (v < u) {
                    continue; 
                }
                if (C - 2 * u - 2 * v < u) {
                    continue; 
                }
                eval(u, v); 
            }
        } 
        ll vmax = min(V, (C - 2) / 2); 
        if (vmax >= 1) {
            ll H = C / 2; 
            vector<ll> cand; 
            add(cand, 1); 
            add(cand, vmax); 
            add(cand, U); 
            add(cand, H - U); 
            add_frac(cand, H, 2); 
            add_frac(cand, B + 2, 2); 
            add_frac(cand, C - 2, 2); 
            add_frac(cand, C + 1, 5); 
            add_frac(cand, C - 3 * U + 1, 2); 
            add(cand, 3 * H - C - 1); 
            add_frac(cand, A + C + 2- 2 * U, 4); 
            add_frac(cand, A + B + C + 4, 8); 
            add_frac(cand, A + C, 4); 
            REP(r, 3) {
                add_frac(cand, 3 * A  - 2 * B + C + 2 * r - 4, 4); 
            }
            for (ll v : cand) {
                if (v < 1 || v > vmax) {
                    continue; 
                }
                ll l = max(1LL, (C -2 * v) / 3 + 1); 
                ll r=min({U, v, H - v}); 
                if (l > r) {
                    continue; 
                }
                ll u = B + 2 - 2 * v >= 0 ? r : l; 
                ll rem = C - 2 * u - 2 * v; 
                if (rem < 0 || rem >= u) {
                    continue; 
                }
                eval(u, v); 
            }
        }
        return ans; 
    }
    ll solve(ll N, ll M, ll K, ll R, ll C) {
        if (N < 3 || M < 3 || K < 8) {
            return 0; 
        }
        ll X = N - 2; 
        ll Y = M - 2; 
        ll vr = max(R, N - R + 1); 
        ll vc = max(C, M - C + 1); 
        ll A = vr  - 2; 
        ll B = vc - 2; 
        ll s = (K - 4) / 2; 
        ll ans = 0; 
        chmax(ans, rect(min(X, A), Y, s)); 
        chmax(ans, rect(X, min(Y, B), s));
        ll U = N - vr; 
        ll V = M - vc; 
        ll rem = K - 2 * (A + B) - 4; 
        chmax(ans, calc(A, B, U, V, rem)); 
        chmax(ans, calc(B, A, V, U, rem)); 
        return ans;  
    }
}
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        ll N, M, K, R, C; 
        read(N, M, K, R, C); 
        print(solve(N, M, K, R, C)); 
    }
}