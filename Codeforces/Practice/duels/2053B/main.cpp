#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        int N; 
        read(N); 
        vector<int> L(N), R(N); 
        vector<int> cnt(N * 2 + 2);
        REP(i, N) {
            read(L[i], R[i]); 
            if (L[i] == R[i]) {
                ++cnt[L[i]]; 
            }
        }
        vector<int> pre(2 * N + 2); 
        REP(x, 1, 2 * N + 1) {
            pre[x] = pre[x - 1] + (cnt[x] > 0); 
        }
        string ans; 
        REP(i, N) {
            if (L[i] == R[i]) {
                ans += cnt[L[i]] == 1 ? '1' : '0'; 
                 
            } else {
                int used = pre[R[i]] - pre[L[i] - 1]; 
                int len = R[i] - L[i] + 1; 
                ans += used < len ? '1' : '0'; 
            }
        }
        print(ans); 
    }
} 
