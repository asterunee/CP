#include "library/template.hpp"
#include "library/number/ext_gcd.hpp"
using namespace std;
using namespace suisen;
using ll = long long;
int norm(ll x, ll n) {
    x %= n; 
    if (x < 0) {
        x += n; 
    }
    return x; 
}
void first_run() {
    int T; 
    read(T); 
    LOOP(T) {
        int n; 
        read(n);
        vector<string> a(n); 
        read(a);  
        ll w = 0, sr = 0, sc = 0; 
        REP(r, n) {
            REP(c, n) {
                if (a[r][c] == '#') {
                    ++w; 
                    sr += r; 
                    sc += c; 
                    
                }
            }
        }
        sr %= n; 
        sc %= n; 
        int rx, cx; 
        read(rx, cx); 
        --rx, --cx; 
        int dr  = safe_mod((w % n) * rx - sr, n); 
        int dc  = safe_mod((w % n) * cx - sc, n);
        if (dr == 0 and dc == 0) {
            print(1, 1, 1, 1); 
            continue; 
        } 
        bool found = false; 
        REP(r, n) {
            REP(c, n) {
                if (a[r][c] != '#') {
                    continue; 
                }
                int nr = (r + dr) % n; 
                int nc = (c + dc) % n; 
                if (a[nr][nc] == '.') {
                    print(r + 1, c + 1, nr + 1, nc + 1); 
                    found = true; 
                    break; 
                }
            }
            if (found) {
                break; 
            }
        }
    }
}
void second_run() {
    int T; 
    read(T); 
    LOOP(T) {
        int n; 
        read(n); 
        vector<string> a(n); 
        read(a); 
        ll w = 0, sr = 0, sc = 0; 
        REP(r, n) {
            REP(c, n) {
                if (a[r][c] == '#') {
                    ++w; 
                    sr += r; 
                    sc += c; 
                    
                }
            }
        }
        sr %= n; 
        sc %= n; 
        ll iv = inv_mod(w % n, n); 
        int rx = sr * iv % n; 
        int cx = sc * iv % n; 
        print(rx + 1, cx + 1); 
    }
}

/*
void solve() {
    
}
*/

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string type; 
    read(type); 
    if (type == "first") {
        first_run(); 
    } else {
        second_run(); 
    }
    /*
    int T;
    read(T);
    LOOP(T) {
        solve();
    }
    */
    return 0;
}