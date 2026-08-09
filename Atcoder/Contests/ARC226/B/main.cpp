#include "library/template.hpp"
#include "library/number/modint.hpp"
using namespace std;
using namespace suisen;
using ll = long long; 
signed main() {
    int T; 
    read(T); 
    while (T--) {
        ll N; 
        int M; 
        read(N, M); 
        vector<ll> A(M); 
        read(A); 
        ll s = 0, ans = 0; 
        RREP(i, M) {
            s = A[i] + 2 * s; 
            chmax(ans, ((s + N - 1) / N) << i); 
        }
        print(ans); 
    }
}