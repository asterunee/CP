#include "library/template.hpp"
#include "library/util/search.hpp"
using namespace std;
using namespace suisen;
using ll = long long;

void solve() {
    int n; 
    read(n); 
    vector<ll> a(n); 
    read(a); 
    sort(ALL(a)); 
    auto ok = [&](int t) {
        int big = max(0, t - 30); 
        if (big >= n) {
            return true; 
        }
        int rem = n - big; 
        max_priority_queue<ll> pq; 
        REP(i, rem) {
            pq.push(a[i]); 
        }
        RREP(k, min(t, 30)) {
            ll x = pq.top(); 
            pq.pop(); 
            x -= 1LL << k; 
            if (x > 0) {
                pq.push(x); 
            }
            if (pq.empty()) {
                return true; 
            }
        }
        return false; 
    }; 
    print(suisen::binary_search(n -1, n + 30, ok)); 
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    read(T);
    LOOP(T) {
        solve();
    }
    return 0;
}