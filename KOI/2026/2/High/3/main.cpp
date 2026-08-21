#include "library/template.hpp"
#include "library/datastructure/segment_tree/segment_tree.hpp"
using namespace std; 
using namespace suisen; 
using P = pair<int, int>; 
P op(P a, P b) {
    return {min (a.first, b.first), max(a.second, b.second)}; 
}
P e() {
    return {int(1e9), -1}; 
}


signed main() {
    int N, M; 
    read(N, M); 
    vector<int> L(N - 1), R(N - 1); 
    REP(i, N - 1) {
        L[i] = R[i] = i; 
    }    
    LOOP(M) {
        int x, y; 
        read(x, y); 
        --x, --y; 
        if (x < y) {
            if (x) {
                chmax(R[x - 1], y - 1); 
            }
            
        } else {
            if (x + 1 < N) {
                chmin(L[x], y); 
            }
        }
    }
    for (int k = 0; (1 << k) < N; ++k) {
        vector<P> a(N - 1); 
        REP(i, N - 1) {
            a[i] = {L[i], R[i]}; 
        }
        SegmentTree<P, op, e> seg(a); 
        vector<int> nL(N - 1), nR(N - 1); 
        REP(i, N - 1) {
            auto [l, r] = seg.prod(L[i], R[i] + 1); 
            nL[i] = l; 
            nR[i] = r; 
        }
        L.swap(nL); 
        R.swap(nR); 
    }
    vector<P> a(N - 1); 
    REP(i, N - 1) {
        a[i] = {L[i], R[i]}; 
    }
    SegmentTree<P, op, e> seg(a); 
    int Q; 
    read(Q); 
    LOOP(Q){ 
        int a, b, c, d; 
        read(a, b, c, d); 
        --a, --b, --c, --d; 
        auto [l, r] = seg.prod(a, b); 
        print (l <= c and d <= r + 1 ? "YES" : "NO"); 
    }
}