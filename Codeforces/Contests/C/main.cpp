#include "library/template.hpp"

using namespace std;
using namespace suisen;
using ll = long long; 

signed main() {
    int T; 
    read(T); 
    while (T--) {
        int N, M, X, Y; 
        read(N, M, X, Y); 
        vector<int> A(X), B(Y); 
        read(A); 
        read(B); 
        int i = X - 1, j = Y - 1; 
        int ca = 0, cb = 0, cnt = 0; 
        int lim = N + M - 1; 
        ll ans = 0; 
        while ((i >= 0 or j >= 0) and cnt < lim) {
            if (j < 0 or (i >= 0 and A[i] > B[j])) {
                if (ca < N) {
                    ans += A[i]; 
                    ++ca; 
                    ++cnt; 
                }
                --i; 
            } else if (i < 0 or B[j] > A[i]) {
                if (cb < M) {
                    ans += B[j]; 
                    ++cb; 
                    ++cnt; 
                }
                --j; 
            } else {
                ans += A[i]; 
                ++cnt; 
                --i; 
                --j; 
            }
        }
        print(ans); 
    } 
}