#include "library/template.hpp"
#include "library/number/modint.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
using mint = modint1000000007; 
signed main() {
    string S; 
    read(S); 
    array<mint, 13> dp{}; 
    dp[0] = 1; 
    for (char c : S) {
        array<mint, 13> ndp{}; 
        REP(r, 13) {
            if (c == '?') {
                REP(d, 10) {
                    ndp[(r * 10 + d) % 13] += dp[r]; 
                }
            } else {
                int d = c - '0'; 
                ndp[(r * 10 + d) % 13] += dp[r]; 
            }
            
        }
        dp = ndp; 
    }
    print(dp[5].val()); 
}