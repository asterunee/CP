#include "library/template.hpp"

using namespace std; 
using namespace suisen; 
using ll = long long; 
signed main() {
    int T; 
    read(T); 
    while (T--) {
        int N, M; 
        read(N, M); 
        vector<ll> V(N); 
        read(V); 
        vector<int> A(N * M); 
        read(A); 
        int ans = M; 
        vector<int> best; 
        best.reserve(M); 
        RREP(i, N) {
            vector<int> cur(M); 
            REP(j, M)  {
                cur[j] = A[i * M +  j]; 
            }
            sort(ALL(cur), greater<int>()); 
            vector<int> nxt; 
            nxt.reserve(M); 
            int p = 0, q = 0; 
            while (int(nxt.size()) < M) {
                if (p == int(best.size())) {
                    nxt.push_back(cur[q++]); 
                } else if (q == M) {
                    nxt.push_back(best[p++]); 
                } else if (best[p] > cur[q]) {
                    nxt.push_back(best[p++]); 
                } else {
                    nxt.push_back(cur[q++]); 
                }
            }
            best.swap(nxt); 
            ll sum = 0; 
            REP(j, min(ans, M)) {
                sum += best[j]; 
                if (sum >= V[i]) {
                    chmin(ans, j + 1); 
                    break; 
                }
            }
        }
        
        print(ans); 
    }
}