#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
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
        vector<int> A(M); 
        read(A); 
        sort(ALL(A)); 
        vector<array<int, 6>> ans(N); 
        for (int i = 0; i < N; i += 2) {
            int l = i / 2; 
            int r = M - 1 - l; 
            REP(j, 6) {
                ans[i][j] = j & 1 ? A[r] : A[l]; 
                if (i + 1 < N) {
                    ans[i + 1][j] = j & 1 ? A[l] : A[r]; 
                }
            }
        }
        REP(i, N) {
            REP(j, 6) {
                if (j) {
                    cout << ' '; 
                }
                cout << ans[i][j]; 
            }
            cout << '\n'; 
        }
    }
} 
