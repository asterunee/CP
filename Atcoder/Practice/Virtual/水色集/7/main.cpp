#include "library/template.hpp"

using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int N; 
    read(N); 
    unordered_map<ll, ll> cnt; 
    cnt.reserve(2 * N); 
    cnt.max_load_factor(0.7); 
    ll s = 0, ans = 0; 
    cnt[0] = 1; 
    REP(i, N) {
        ll A; 
        read(A); 
        if (i % 2 == 0) {
            s += A; 
        } else {
            s -= A; 
        }
        ans += cnt[s]; 
        ++cnt[s]; 
    }
    print(ans); 
}