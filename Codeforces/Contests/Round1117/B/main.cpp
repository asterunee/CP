#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        int N, M; 
        read(N, M); 
        vector<ll> A(N), B(M); 
        read(A,B); 
        ll a = A[0] + N - 1; 
        ll b = B[0] + M - 1; 
        print(b <= a ? 1 : 2); 
    }
}