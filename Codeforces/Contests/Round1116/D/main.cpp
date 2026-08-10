#include "library/template.hpp"
#include "library/number/modint.hpp"
#include "library/math/factorial.hpp"

using namespace std;
using namespace suisen;
using ll = long long;
using mint = modint998244353; 
factorial<mint> fac; 
mint comp(int n, int k) {
    if (k == 0) {
        return n == 0; 
    } 
    if (n < k) {
        return 0; 
    }
    return fac.binom(n - 1, k - 1); 
}
void solve() {
    int n; 
    string s; 
    read(n, s); 
    int one = count(s.begin(), s.end(), '1'); 
    int tr = 0; 
    REP(i, n - 1) {
        tr += s[i] != s[i + 1]; 
    }
    int runs = tr + 1; 
    int r0 = runs / 2; 
    int r1 = runs / 2; 
    if (runs & 1) {
    if (s[0] == '0') {
        ++r0; 
    } else {
        ++r1; 
    }
}
    print(comp(one, r1) * comp(n - one, r0)); 
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