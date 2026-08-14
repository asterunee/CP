#include "library/template.hpp"
#include "library/number/modint.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
using mint = modint998244353; 
signed main() {
    int A, B, C, D; 
    read(A, B, C, D); 
    vector<mint> pre(D + 1), cur(D + 1); 
    REP(i, A, C + 1) {
        fill(ALL(cur), mint(0)); 
        REP(j, B, D + 1) {
            if (i == A && j == B) {
                cur[j] = 1; 
                continue; 
            }
            if (i > A) {
                cur[j] += pre[j] * j; 
            }
            if (j > B) {
                cur[j] += cur[j - 1] * i; 
            }
            if (i > A && j > B) {
                cur[j] -= pre[j - 1] * (i - 1) * (j - 1); 
            }
        }
        pre.swap(cur); 
    }
    print(pre[D].val()); 
}