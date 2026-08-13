#include "library/template.hpp"
#include "library/number/modint.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using mint = modint998244353; 
struct Seg {ll rem, l, r; }; 

signed main() {
    ll N, X, D; 
    read(N, X, D); 
    if (D == 0) {
        print(X == 0 ? 1 : N + 1); 
        return 0; 
    }
    if (D < 0) {
        X = -X; 
        D = -D; 
    }
    ll g = std::gcd(abs(X), D); 
    X /= g; 
    D /= g; 
    vector<Seg> seg;
    seg.reserve(N + 1); 
    REP(k, N + 1) {
        ll l = k * X + D * k * (k - 1) / 2; 
        ll r = k * X + D * k * (2 * N - k - 1) / 2; 
        ll rem = (l % D  + D) % D; 
        l = (l - rem) / D;
        r = (r - rem) / D; 
        seg.push_back({rem, l, r}); 
    } 
    sort(ALL(seg), [](const Seg& a, const Seg& b) {if (a.rem != b.rem) return a.rem < b.rem; return a.l < b.l; });
    ll ans = 0; 
    for (int i = 0; i < int(seg.size()); ) {
        int j = i; 
        ll cur = seg[i].l - 1; 
        while (j < int(seg.size()) && seg[j].rem == seg[i].rem) {
            ll l = seg[j].l; 
            ll r = seg[j].r; 
            if (r > cur) {
                ans += r - max(cur + 1, l) + 1; 
                chmax(cur, r); 
            }
            ++j; 
        }
        i = j; 
    } 
    print(ans); 
}