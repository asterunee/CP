#include "library/template.hpp"
#include "library/number/modint.hpp"
using namespace std;
using namespace suisen;
using mint = modint998244353; 
signed main() {
    int N; 
    read(N); 
    vector<int> ev(N * 2 + 1); 
    REP(i, N) {
        int S, T; 
        read(S, T); 
        ev[S] = 1; 
        ev[T] =-1; 
    }
    int active = 0, c = 0; 
    REP(t, 1, 2 * N + 1) {
        if (ev[t] == 1) {
            if (active == 2) {
                print(0); 
                return 0; 
            }
            if(active == 0) {
                ++c; 
            }
            ++active; 
        } else {
            --active; 
        }
    }
    print(mint(2).pow(c).val()); 
}