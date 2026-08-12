#include "library/template.hpp"
#include "library/number/modint.hpp"
#include "library/math/factorial.hpp"

using namespace std; 
using namespace suisen; 
using ll = long long; 
using mint = modint1000000007; 

signed main() {
    int T; 
    read(T); 
    while (T--) {
        int N; 
        string S; 
        read(N, S); 
        int l = count(ALL(S), 'L'); 
        int m = count(ALL(S), 'M'); 
        if (l > N || m > N) {
            print(-1); 
            continue; 
        }
        int need = N - m; 
        ll ans = 0; 
        REP(i, 2 * N) {
            if (S[i] == 'M') {
                ans += 2LL * (i + 1); 
            } else {
                ans += i + 1; 
            }
        }
        REP(i, 2 *N) {
            if (S[i] == '?' && need) {
                ans += i + 1; 
                --need; 
            }
        }
        print(ans); 
    }
}