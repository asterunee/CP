#include "library/template.hpp"

using namespace std; 
using namespace suisen; 
using ll = long long; 
signed main() {
    int T; 
    read(T); 
    while (T--) {
        int N; 
        string S; 
        read(N, S); 
        int c0 = count(ALL(S), '0'); 
        int c1 = N - c0; 
        int d = c0 -c1; 
        if (abs(d) > 2) {
            print(-1); 
            continue; 
        }
        int R = 1; 
        REP(i, 1, N) {
            R += S[i] != S[i - 1]; 
        }
        int best[3]{}; 
        if (R % 2 == 0) {
            best[0] = R - 1; 
            best[1] = R; 
            best[2] = R - 1; 
        } else if (S[0] == '0') {
            best[0] = max(0, R - 2); 
            best[1] = R - 1; 
            best[2] = R; 
        } else {
            best[0] = R; 
            best[1] = R - 1; 
            best[2] = max(0, R - 2); 
        }
        int keep = 0; 
        REP(e, -1, 2) {
            if (abs(d - e) <= 1) {
                chmax(keep, best[e + 1]); 
            }
        }
        print(N - keep); 
        
    }
}