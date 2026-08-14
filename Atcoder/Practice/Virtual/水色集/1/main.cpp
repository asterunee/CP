#include "library/template.hpp"
#include "library/datastructure/union_find/union_find.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    int N, M; 
    read(N, M); 
    vector<int> A(M), B(M); 
    REP(i, M) {
    read(A[i], B[i]); 
    --A[i], --B[i]; 
    }
    UnionFind uf(N); 
    vector<ll> ans(M); 
    ll cur = 1LL * N * (N - 1) / 2; 
    RREP(i, M) {
        ans[i] = cur; 
        int a = A[i]; 
        int b = B[i]; 
        if (uf.same(a, b)) {
            continue; 
        } 
        cur -= 1LL * uf.size(a) * uf.size(b); 
        uf.merge(a, b); 
    }
    FOR(x, ans) {
        print(x); 
    }
}