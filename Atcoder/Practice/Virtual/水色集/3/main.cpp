#include "library/template.hpp"
#include "library/number/modint.hpp"
#include "library/datastructure/union_find/union_find.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using mint = modint998244353; 

signed main() {
    int N, K; 
    read(N, K); 
    vector<vector<int>> A(N, vector<int> (N)); 
    read(A); 
    UnionFind row(N), col(N); 
    REP(x, N) {
        REP(y, x + 1, N) {
            bool ok = true; 
            REP(i, N) {
                if (A[x][i] + A[y][i] > K) {
                    ok = false; 
                    break; 
                }
            }
            if (ok) {
                row.merge(x, y); 
            }
        }
    }
    REP(x, N) {
        REP(y, x + 1, N) {
            bool ok = true; 
            REP(i, N) {
                if (A[i][x] + A[i][y] > K) {
                    ok =false; 
                    break; 
                }
            }
            if (ok) {
                col.merge(x, y); 
            }
        }
    }
    vector<mint> fac(N + 1); 
    fac[0] = 1; 
    REP(i, 1, N + 1) {
        fac[i] = fac[i - 1] * i; 
    }
    mint ans = 1; 
    REP(i, N) {
        if (row.root(i) == i) {
            ans *= fac[row.size(i)]; 
        }
        if (col.root(i) == i) {
            ans *= fac[col.size(i)]; 
        }
    }
    print(ans.val()); 
}