#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        string S; 
        read(S); 
        int l = count(ALL(S), 'L'); 
        int r = count(ALL(S), 'R'); 
        int u = count(ALL(S), 'U'); 
        int d = count(ALL(S), 'D'); 
        int x  = min(l, r); 
        int y = min(u, d); 
        if (x == 0 and y == 0) {
            print(0); 
            cout << '\n'; 
        } else if (x == 0) {
            print(2); 
            cout << "UD\n"; 
        } else if(y == 0) {
            print(2); 
            cout << "LR\n" ;
        } else {
            print(2 * (x + y)); 
            cout << string(x, 'L') << string(y, 'U') << string(x, 'R') << string(y, 'D') << '\n';
        }
    }
} 