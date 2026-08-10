#include "library/template.hpp"
#include "library/util/search.hpp"

using namespace std;
using namespace suisen;
using ll = long long; 

signed main() {
    int T; 
    read(T); 
    while (T--) {
        ll X,Y; 
        read(X, Y); 
        auto tri = [&](ll k) {
            return k * (k + 1 ) / 2; 
        };
        int K = suisen::binary_search<ll> (0, 20001, [&](ll k) {return tri(k) > X + Y; }) - 1;
        ll best = X *X + Y * Y; 
        int bk = 0; 
        ll bp = 0; 
        REP(k, 1, K + 1) {
            ll s = tri(k); 
            ll l = max(0LL, s - Y); 
            ll r = min(X, s); 
            auto dist = [&](ll p) {
                ll q = s - p; 
                return (X - p) * (X - p) + (Y - q) * (Y - q); 
            }; 
            ll p = ternary_search_key<ternary_search_tag::Convex>(l, r, dist); 
            ll cur = dist(p); 
            if (chmin(best, cur)) {
                bk = k; 
                bp = p; 
            }
        }
        string ans(bk, 'Y'); 
        RREP(w, 1, bk + 1) {
            if (w <= bp) {
                bp -= w; 
                ans[bk - w] = 'X'; 
            }
        }
        print(ans); 
    }
}