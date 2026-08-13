#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    ll R, X, Y;
    read(R, X, Y); 
    ll d2 = X * X + Y * Y; 
    ll r2 = R * R; 
    if (d2 == r2 ) {
        print(1); 
        return 0; 
    } 
    if (d2 < r2) {
        print(2) ; 
        return 0; 
    }
    ll k = 1; 
    while (k * k * r2 < d2) {
        ++k; 
    }
    print(k); 
}