#include "library/template.hpp"
#include "library/number/kth_root_round.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        ll S, Q; 
        read(S, Q); 
        vector<ll> d; 
        ll r= floor_kth_root(S, 2); 
        REP(i, 1LL, r + 1) {
            if (S % i) {
                continue; 
            }
            d.push_back(i); 
            if (i * i != S) {
                d.push_back(S / i); 
            }
        }
        sort(ALL(d)); 
        int n = d.size(); 
        vector<ll> pre(n + 1); 
        REP(i, n) {
            ll l = i ? d[i - 1] : 0; 
            pre[i + 1] = pre[i] + (d[i] - l) * (S / d[i]); 
        }
        auto sum = [&](ll x) {
            int i = lower_bound(ALL(d), x) - d.begin(); 
            ll l = i ? d[i - 1] : 0; 
            return pre[i] + (x - l) * (S / d[i]); 
        }; 
        LOOP(Q) {
            ll x, y; 
            read(x, y); 
            ll v = S/ y; 
            int i = upper_bound(ALL(d), v) - d.begin() - 1; 
            ll z = d[i]; 
            if (x <= z) {
                print(x * y); 
                continue; 
            }
            print(z * y + sum(x) - sum(z)); 
        }
    }
}