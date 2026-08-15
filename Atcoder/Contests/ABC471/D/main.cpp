#include "library/template.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 

signed main() {
    int Q; 
    ll V; 
    read(Q, V); 
    priority_queue<ll> pq; 
    REP(_, Q) {
        int type; 
        ll t; 
        read(type, t); 
        if (type == 1) {
            ll w; 
            read(w); 
            pq.push(w - t); 
        } else {
            if (pq.empty()) {
                print(-1); 
                continue;
            }
            ll x = pq.top(); 
            pq.pop(); 
            print(min(V, t + x)); 
        }
    }
}