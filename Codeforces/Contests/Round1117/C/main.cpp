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
        vector<int> p(N + 1); 
        REP(i, 2, N + 1) {
            read(p[i]); 
        }
        int M; 
        read(M); 
        vector<int> A(M); 
        read(A); 
        int z = *min_element(ALL(A)); 
        vector<int> ans; 
        ans.reserve(M  -1); 
        for (int x : A) {
            if (x != z) {
                ans.push_back(x); 
            }
        
        }
        cout << ans.size(); 
        for (int x : ans) {
            cout << ' ' << x; 
        }
        cout << '\n'; 
    }
}