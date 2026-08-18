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
        ll N, X; 
        read(N, X); 
        vector<ll> A(N); 
        read(A); 
        sort(ALL(A)); 
        ll sum = 0; 
        for (ll x : A) {
            sum += x; 
        }
        int K = sum / X; 
        ll score = 0; 
        REP(i, N- K, N) {
            score += A[i]; 
        }
        vector<ll> ans; 
        ans.reserve(N); 
        int p = 0; 
        ll rem = 0; 
        REP(i, N - K, N) {
            ll x = A[i]; 
            while (rem + x < X) {
                rem += A[p]; 
                ans.push_back(A[p++]); 
            }
            ans.push_back(x); 
            rem += x -X; 
        }
        while (p < N - K) {
            ans.push_back(A[p++]); 
        }
        print(score); 
        REP(i, N) {
            if (i) {
                cout << ' '; 
            }
                            cout << ans[i]; 

            
        }
        cout << '\n'; 
    }
} 
