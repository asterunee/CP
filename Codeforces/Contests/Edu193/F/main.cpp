#include "library/template.hpp"
#include "library/transform/subset.hpp"
#include "library/transform/supset.hpp"
using namespace std;
using namespace suisen;
using ll = long long; 
ll mn(ll x, ll y) {
    return min(x, y); 
}
signed main() {
        int N; 
        read(N); 
        vector<ll> M(N + 1); 
        REP(i, 1, N + 1) {
            read(M[i]); 
        }
        ll ans= 0; 
        for (int r = 1; r <= N / 4; r += 2) {
            if (r % 3 == 0) {
                continue; 
            }
            vector<int> base; 
            for (ll x = r; x <= N; x *= 3) {
                base.push_back(x); 
                if (x > N /3) {
                    break; 
                }
            }
            vector<ll> dp; 
            for (int z= int(base.size()) - 1; z >= 0; --z) {
                int s= base[z]; 
                int L = 0; 
                for (ll x = s; x <= N; x *= 2) {
                    ++L; 
                    if (x > N / 2) {
                        break; 
                    }
                }
                int S = 1 << L; 
                vector<ll> cur(S); 
                vector<ll> w(L); 
                ll x = s; 
                REP(i, L) {
                    w[i] = M[x]; 
                    x *= 2; 
                }
                REP(i, L) {
                    cur[1 << i] = w[i]; 
                }
                subset_transform::zeta(cur); 
                if (!dp.empty()) {
                    supset_transform::zeta<ll, mn>(dp); 
                    int q = max(0, L -2); 
                    int all = (1 << q) - 1; 
                    REP(mask, S) {
                        int need = (~(mask | (mask >> 1) | (mask >> 2))) & all;
                        cur[mask] += dp[need];  
                    }
                }
                dp = move(cur); 
            }
            ans += *min_element(ALL(dp)); 
        }
         print(ans); 
}