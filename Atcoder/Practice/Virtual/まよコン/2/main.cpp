#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    int N; 
    read(N); 
    vector<string> S(N); 
    vector<int> P(N); 
    REP(i, N) {
        read(S[i], P[i]); 
    }
    vector<int> ord(N); 
    iota(ALL(ord), 0); 
    sort(ALL(ord), [&](int i, int j) {if (S[i] != S[j]) return S[i] < S[j]; return P[i] > P[j]; });
    FOR(i, ord) {
        print(i + 1); 
    } 
}