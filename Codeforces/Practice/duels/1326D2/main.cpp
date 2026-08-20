#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
int palpre(const string & s) {
    string r = s; 
    reverse(ALL(r)); 
    string t = s + '#' + r; 
    vector<int> p(t.size()); 
    REP(i, 1, (int)t.size()) {
        int j = p[i - 1]; 
        while (j and t[i] != t[j]) {
            j = p[j - 1]; 
        }
        if (t[i] == t[j]) {
            ++j; 
        }
        p[i] = j; 
    }
    return p.back(); 
}
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        string S; 
        read(S); 
        int N = S.size(); 
        int l = 0; 
        while (l < N - 1 - l and S[l] == S[N - 1 - l]) {
            ++l; 
        }
        if (2 * l >= N) {
            print(S); 
            continue; 
        }
        string mid = S.substr(l, N - 2 * l); 
        int a = palpre(mid); 
        string rev = mid; 
        reverse(ALL(rev)); 
        int b = palpre(rev); 
        string ans = S.substr(0, l); 
        if (a >= b) {
            ans += mid.substr(0, a); 
        } else {
            ans += mid.substr(mid.size() - b); 
        }
        ans += S.substr(N - l); 
        print(ans); 
    }
} 