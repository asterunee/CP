#include "library/template.hpp"
#include <atcoder/convolution>
#include "library/number/modint.hpp"
using namespace std; 
using namespace suisen; 
using namespace atcoder; 
using ll = long long; 
using ull = unsigned long long; 
using mint = atcoder::modint998244353; 
signed main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(NULL); 
    int N, K, M;
    ull seed; 
    read(N, K, seed, M); 
    vector<int> b(M), v(K); 
    read(b, v); 
    vector<unsigned int> cnt(K), tr(K * K), dc(K); 
    int prv = -1; 
    auto add = [&](int a) {++cnt[a]; if (prv >= 0) {int d = a - prv; if (d < 0) {d += K; }++tr[d * K + prv]; ++dc[d]; } prv = a; };
    REP(i, M) {
        add(b[i]); 
    } 
    ull state = seed; 
    REP(i, M, N) {
        uint32_t x = uint32_t(((state >> 18) ^ state) >> 27); 
        unsigned int r = state >> 59; 
        uint32_t y = (x >> r) | (x << ((32 - r) & 31)); 
        add(y % K); 
        state = state * 6364136223846793005ULL +  2026081520260815ULL;
    }
    vector<ll> ans(K); 
    REP(a, K) {
        if (cnt[a]) {
            REP(x, K) {
                if (v[x]) {
                    int k = x - a; 
                    if (k < 0) {
                        k += K; 
                    }
                    ans[k] += cnt[a]; 
                }
            }
        }
    }
            vector<mint> f(K), g(2 * K); 
            REP(d, K) {
                if (dc[d]) {
                    bool ok = false; 
                    REP(a, K) {
                        f[K - 1 - a] = tr[d * K + a]; 
                        int z = v[a] && v[(a + d) % K];
                        g[a] = z; 
                        g[a + K] =z; 
                        ok |= z;  
                    }
                    if (!ok) {
                        continue; 
                    }
                    auto h = atcoder::convolution(f, g); 
                    REP(k, K) {
                        ans[k] -= h[K - 1 + k].val(); 
                    }
                }
            }
            REP(k, K) {
                print(ans[k]); 
            }
}