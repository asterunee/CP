#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int N; 
    read(N); 
    string A, B, C; 
    read(A, B, C); 
    vector<int> a, b, c; 
    a.reserve(N); 
    b.reserve(N); 
    c.reserve(N); 
    REP(i, 2 * N) {
        if (A[i] == '1') {
            a.push_back(i); 
        }
        if (B[i] == '1') {
            b.push_back(i); 
        }
        if (C[i] == '1') {
            c.push_back(i); 
        }
    }
    vector<int> x(N); 
    ll ans = 0; 
    REP(i, N) {
        array<int, 3> p = {a[i], b[i], c[i]}; 
        sort(ALL(p)); 
        x[i] = p[1]; 
        ans += abs(a[i] - x[i]); 
        ans += abs(b[i] - x[i]); 
        ans += abs(c[i] - x[i]); 
    }
    string X(2 * N, '0'); 
    for (int p : x) {
        X[p] = '1'; 
    }
    print(ans); 
    print(X); 
}