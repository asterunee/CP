#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    int N, M; 
    read(N, M); 
    vector<vector<int>> job(M + 1); 
    REP(i, N) {
        int A, B; 
        read(A, B); 
        if (A <= M) {
            job[A].push_back(B); 
        }
    }
    priority_queue<int> pq; 
    ll ans= 0; 
    REP(d, 1, M + 1) {
        FOR(B, job[d]) {
            pq.push(B); 
        }
        if (not pq.empty()) {
            ans += pq.top(); 
            pq.pop(); 
        }
    }
    print(ans); 
}