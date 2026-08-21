#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ld = long double; 
signed main() {
    int N; 
    read(N); 
    vector<ll> A(N); 
    read(A); 
    sort(ALL(A)); 
    auto med = [&](int l, int r) -> ld{
        int n = r - l; 
        if (n & 1) {
            return A[l + n / 2]; 
        }
        return (A[l + n / 2 - 1] + A[l + n / 2]) / 2.0L; 
    }; 
    int k = N / 2; 
    ld q1 = med(0, k);
    ld q2 = med(0, N);
    ld q3 = med(N - k, N);
    ld iqr = q3 - q1;
    ld lo = q1 - 1.5L * iqr;
    ld hi = q3 + 1.5L * iqr;
    int U = 0;
    for (auto x : A) {
        if (x < lo or x > hi) {
            ++U;
        }
    }
    cout << fixed << setprecision(10) << q1 << ' ' << q2 << ' ' << q3 << ' ' << U << '\n';
}