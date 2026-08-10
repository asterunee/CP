#include "library/template.hpp"

using namespace std;
using namespace suisen;
using ll = long long;

void solve() {
    int n; 
    ll k; 
    string s; 
    read(n, k, s); 
    int m = 2 * n; 
    int tot = 0, red = 0; 
    REP(i, m) {
        if (s[i] == '1') {
            ++tot; 
            if (i & 1) {
                ++red; 
            }
        }
        
    }
    REP(i, m) {
            int j = (i + 1) % m; 
            if (s[i] == '1' and s[j] == '0') {
                if (i & 1) {
                    --red; 
                } else {
                    ++red; 
                }
            }
        }
    print(red, tot - red); 
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