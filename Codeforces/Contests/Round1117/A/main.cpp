#include "library/template.hpp"
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
        array<bool , 26> ok{}; 
        REP(_, N) {
            string s; 
            read(s); 
            ok[s[0] - 'a'] = true; 
        }
        bool ans = true; 
        REP(_, M) {
            string s; 
            read(s); 
            FOR(c, s) {
                if (!ok[c - 'A']) {
                    ans = false; 
                }
            }
        }
        print(ans ? "YES" : "NO"); 
    }
}