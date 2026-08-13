#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    int N; 
    read(N); 
    vector<int> A(N); 
    read(A); 
    int ans = (1 << 30) - 1; 
    REP(mask, 1 << (N - 1)) {
        int xr = 0; 
        int cur = 0; 
        REP(i, N) {
            cur |= A[i]; 
            if (i == N - 1 || (mask >> i & 1)) {
                xr ^= cur; 
                cur = 0; 
            }
        }
        chmin(ans, xr); 
    }
    print(ans); 
}