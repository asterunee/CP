#include "library/template.hpp"
#include "library/datastructure/union_find/union_find.hpp"
#include "library/number/modint.hpp"
#include "library/math/factorial.hpp"
using namespace std;
using namespace suisen;
using mint = modint998244353;
signed main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(NULL); 
    int N, M; 
    string S; 
    read(N, M, S); 
    UnionFind uf(N); 
    REP(_, M) {
        int a,b; 
        read(a, b); 
        --a, --b; 
        uf.merge(a, b); 
    }
    factorial<mint> fac(N); 
    mint ans = 1; 
    bool dup = false; 
    for (auto& g : uf.groups()) {
        array<int, 26> cnt{}; 
        for (int v : g) {
            ++cnt[S[v] - 'a']; 
        }
        ans *= fac.fac(g.size()); 
        for (int c : cnt) {
            ans *= fac.fac_inv(c); 
            dup |= c >= 2; 
        }
    }
    if (!dup) {
        ans /= 2; 
    }
    print(ans.val()); 
}