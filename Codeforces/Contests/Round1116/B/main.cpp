#include "library/template.hpp"

using namespace std;
using namespace suisen;
using ll = long long;

void solve() {
    int n; 
    string s; 
    read(n, s); 
    int ans = 0; 
    REP(a , 2) {
        REP(b, 2) {
            bool ok = true; 
            REP(i, n) {
                int x; 
                if (i & 1) {
                    x = b ^ (i / 2 & 1); 
                } else {
                    x = a ^ (i / 2 & 1); 
                }
                if (s[i] != '?' and s[i] - '0' != x) {
                    ok = false; 
                    break; 
                }
            }
            ans += ok; 
        }
    }
    print(ans); 
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