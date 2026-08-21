#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        ll N, K; 
        read(N, K); 
        if (N == K ){
            print("YES"); 
            print(1); 
            print(1); 
        } else if (N >= 2 * K - 1) {
            print("YES"); 
            print(2); 
            print(N - K + 1, 1); 
        } else {
            print("NO"); 
        }
    }
}
 