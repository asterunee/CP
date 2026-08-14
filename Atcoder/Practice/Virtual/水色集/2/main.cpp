#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    int N; 
    read(N); 
    ll cnt[19][19] = {}; 
    REP(_, N) {
        string s; 
        read(s); 
        string a, b; 
        auto p = s.find('.'); 
        if (p == string::npos) {
            a = s; 
            b = ""; 
        } else {
            a = s.substr(0, p); 
            b = s.substr(p + 1); 
        }
        while (b.size() < 9) {
            b += '0'; 
        }
        ll x = stoll(a) * 1000000000LL;
        if (not b.empty()) {
            x += stoll(b); 
        } 
        int p2 = 0, p5 = 0; 
        while (p2 < 18 && x % 2 == 0) {
            x /= 2; 
            ++p2; 
        } 
        while (p5 < 18 && x % 5 == 0) {
            x /= 5; 
            ++p5; 
        }
        ++cnt[p2][p5]; 
    }
    ll ans = 0; 
    REP(a, 19) {
        REP(b, 19) {
            int x = a * 19 + b; 
            REP(c, 19) {
                REP(d, 19) {
                    int y = c * 19 + d; 
                    if (x > y) {
                        continue; 
                    }
                    if (a + c < 18 || b + d < 18) {
                        continue; 
                    }
                    if (x == y) {
                        ans += cnt[a][b] * (cnt[a][b] - 1) / 2; 
                    } else {
                        ans += cnt[a][b] * cnt[c][d]; 
                    }
                }
            }
        }
    }
    print(ans); 
}