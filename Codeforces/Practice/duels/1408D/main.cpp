#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int N, M; 
    read(N, M); 
    vector<pair<int, int>> A(N), B(M); 
    read(A, B); 
    constexpr int L = 1000002; 
    vector<int> mx(L + 2); 
    for (auto [a, b] : A) {
        for (auto [c, d] : B) {
            if(a <= c and b <= d) {
                chmax(mx[c - a + 1], d - b + 1); 
            }
        }
    }
    RREP(i, L + 1) {
        chmax(mx[i], mx[i + 1]); 
    }
    int ans = L * 2; 
    REP(x, L) {
        chmin(ans, x + mx[x + 1]); 
    }
    print(ans);
} 