#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
constexpr int W = 1000; 
signed main() {
    int N,M, Q; 
    read(N, M, Q); 
    vector<string> S(N); 
    read(S); 
    vector<bitset<W>> ok(2 * M); 
    for (auto s : S) {
        bitset<W> b; 
        REP(i, M) {
            b.set(2 * i + s[i] - '0'); 
        }
        REP(i, M) {
            ok[2 * i + s[i] - '0'] |= b; 
        }
    }
    LOOP(Q) {
        string T; 
        read(T); 
        bitset<W> b; 
        REP(i, M) {
            b.set(2 * i + T[i] - '0'); 
        }
        bool ans = true; 
        REP(i, M) {
            int x = 2 * i + T[i] - '0'; 
            if ((b & ~ok[x]).any()) {
                ans = false; 
                break; 
            }
        }
        print(ans ? "Yes" : "No"); 
    }
}