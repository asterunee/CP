#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    int N; 
    read(N); 
    map<string, int> cnt; 
    int ans = 0; 
    REP(_, N) {
        string S; 
        read(S); 
        FOR(c, S) {
            c = tolower(c); 
        }
        chmax(ans, ++cnt[S]); 
    }
    print(ans); 
}