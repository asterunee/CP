#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
signed main() {
    ll N; 
    read(N); 
    ll cnt = 0; 
    do {
        if (N & 1 ) {
            N = 3 * N + 1; 
        }else {
            N /= 2; 
        }
        ++cnt; 
    } while (N != 1);
    print(cnt); 
}