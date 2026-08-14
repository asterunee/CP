#include "library/template.hpp"

using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    while (T--) {
        int N; 
        read(N); 
        int xr = 0, o = 0; 
        REP(i, N) {
            int A; 
            read(A); 
            xr ^= A; 
            o |= A; 
        }
        print(xr == o ? "Bob" : "Alice"); 
    }
}