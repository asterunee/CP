#include "library/template.hpp"

using namespace std; 
using namespace suisen; 
using ll = long long; 
signed main() {
    int T; 
    read(T); 
    while (T--) {
        int N; 
        read(N); 
        vector<int> A(N); 
        read(A); 
        map<int, int> cnt; 
        ll ans = 0; 
        for (int x : A) {
            ++cnt[x]; 
            ans += x; 
        }
        for (auto [x, c]  : cnt) {
            int rest = N - c; 
            if (c > rest + 2) {
                ans -= 1LL * (c - rest - 2) * x; 
                break; 
            }
        }
        print(ans); 
    }
}