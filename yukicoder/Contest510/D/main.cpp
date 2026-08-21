#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ld = long double; 
using i128 = __int128_t;
ll pw(ll a, ll b, ll mod) {
    ll res = 1 % mod;
    while (b) {
        if (b & 1) res = (i128)res * a % mod;
        a = (i128)a * a % mod;
        b >>= 1;
    }
    return res;
}
signed main() {
    int T;
    read(T);

    LOOP(T) {
        ll A, B, D, R;
        read(A, B, D, R);
        ll S = D + R; 
        i128 lhs = (i128)A * R; 
        i128 rhs = (i128)B * S; 
        if (lhs > rhs) {
            print("rabbit"); 
        } else if (lhs < rhs) {
            print("turtle"); 
        } else {
            ll h = pw(10, 100, S); 
            ll rem = ((i128)h * R + D) % S; 
            if (D > rem) {
                print("rabbit"); 
            }else  if (D < rem) {
                print("turtle"); 
            } else {
                print("tie"); 
            }
        }
        
    }
}