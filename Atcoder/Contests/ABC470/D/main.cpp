#include "library/template.hpp"

using namespace std;
using namespace suisen;

signed main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(NULL); 
    int N, Q; 
    read(N, Q); 
    vector<int> P(N), I(N); 
    read(P); 
    for (int& x : P ) {
        --x; 
    }
    REP(i, N) {
        I[P[i]] = i; 
    }
    bool inv = false; 
    while (Q--) {
        int t;
        read (t); 
        if (t == 2) {
            inv ^= 1; 
            continue; 
        }
        int x, y; 
        read(x, y); 
        --x, --y; 
        if (!inv) {
            int a = P[x]; 
            int b =  P[y]; 
            swap(P[x], P[y]); 
            I[a] = y; 
            I[b] = x; 
        } else {
            int a = I[x]; 
            int b = I[y]; 
            swap(I[x], I[y]); 
            P[a] = y; 
            P[b] =x; 
        }
    }
    vector<int>& ans = inv ? I : P; 
    for (int& x : ans) {
        ++x; 
    }
    print(ans); 
}