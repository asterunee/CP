#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
    int N; 
    read(N); 
    vector<ll> A(N); 
    read(A); 
    ll ans = 1LL << 60; 
    REP(c1, 3) {
        REP(c2, 3) {
            ll c3= 0; 
            bool ok = true; 
            for (ll a : A) {
                ll need = 1LL << 60; 
                REP(x1, c1 + 1) {
                    REP(x2, c2 + 1) {
                        ll rem = a - x1 - 2 * x2; 
                        if (rem >= 0 and rem % 3 == 0) {
                            chmin(need, rem / 3); 
                        }
                    }
                
                }
                if (need == (1LL << 60)) {
                    ok = false; 
                    break; 
                }
                chmax(c3, need); 

            }
            if (ok) {
                chmin(ans, (ll)c1 + c2 + c3); 
            }
        }
        
        
    }
    print(ans); 
}
} 