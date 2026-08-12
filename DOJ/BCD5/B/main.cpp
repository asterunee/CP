#include "library/template.hpp"
#include "library/number/modint.hpp"
#include "library/math/factorial.hpp"

using namespace std; 
using namespace suisen; 
using mint = modint1000000007; 

signed main() {
    int T; 
    read(T); 
    while (T--) {
        int N; 
        read(N); 
        vector<int> A(N); 
        REP(i, N) {
            A[i] = 2 * i + 1; 
        }
        print(A); 
    }
}