#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    int N; 
    read(N) ;
    vector<ll> A(N); 
    read(A); 
    sort(ALL(A)); 
    vector<int> prv(N), nxt(N); 
    REP(i, N) {
        prv[i] = i - 1; 
        nxt[i] = i + 1; 
    }
    int k = lower_bound(ALL(A), 0LL) - A.begin(); 
    int v; 
    if (k == 0) {
        v = 0; 
    } else if (k == N) {
        v = N - 1;  
    } else {
        ll dl = -A[k - 1]; 
        ll dr = A[k]; 
        v = dl <= dr ? k - 1 : k; 
    }
    ll pos = 0, ans = 0; 
    REP(_, N) {
        ans += abs(A[v] - pos); 
        pos = A[v]; 
        int l = prv[v]; 
        int r = nxt[v]; 
        if (l >= 0) {
            nxt[l] = r;  
        } 
        if (r < N) {
            prv[r] = l; 
        }
        if (_ == N - 1) {
            break; 
        }
        if (l < 0) {
            v = r; 
        } else if (r >= N) {
            v = l; 
        } else {
            ll dl = pos - A[l]; 
            ll dr = A[r] - pos; 
            v = dl <= dr ? l : r; 
        }
    }
    print(ans); 
}