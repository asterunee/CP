#include "library/template.hpp"
#include "library/datastructure/segment_tree/segment_tree_beats.hpp"

using namespace std;
using namespace suisen;
using ll = long long; 
constexpr int INF = 1 << 30; 
constexpr int ID = -1; 
struct S{
    ll sum = 0; 
    int mn = INF; 
    int mn2 = INF; 
    int cnt = 0; 
    bool fail = false; 
    S() = default; 
    S(int x) : sum(x), mn(x), cnt(1) {}
}; 
S op(S a, S b) {
    if (a.cnt == 0) {
        return b; 
    }
    if (b.cnt == 0) {
        return a; 
    }
    S c; 
    c.sum = a.sum + b.sum; 
    if (a.mn < b.mn) {
        c.mn = a.mn; 
        c.cnt = a.cnt; 
        c.mn2 = min(a.mn2, b.mn); 
    } else if (a.mn > b.mn) {
        c.mn = b.mn; 
        c.cnt = b.cnt; 
        c.mn2 = min(a.mn, b.mn2); 
    } else {
        c.mn = a.mn; 
        c.cnt = a.cnt + b.cnt; 
        c.mn2 = min(a.mn2, b.mn2); 
    }
    return c; 
}
S e() {
    return S{}; 
}
S mapping(int x, S a) {
    if (a.cnt == 0 or x == ID or x <= a.mn) {
        return a; 
    }
    if (x < a.mn2) {
        a.sum += 1LL * (x - a.mn) * a.cnt; 
        a.mn = x; 
        return a; 
    }
    a.fail = true; 
    return a; 
}
int composition(int f, int g) {
    return max(f, g);
}
int id() {
    return ID; 
}
using Seg = SegmentTreeBeats<S, op, e, int, mapping, composition, id>; 
signed main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(NULL); 
    int N; 
    read(N); 
    vector<int> A(N); 
    read(A); 
    vector<vector<int>> pos(N + 1); 
    REP(i, N) {
        pos[A[i]].push_back(i); 
    }
    vector<S> init(N, S(0)); 
    Seg seg(init); 
    ll ans = 0; 
    ll base = 1LL * N* (N + 1); 
    REP(x, N) {
        if (pos[x].empty()) {
            break; 
        }
        int l = 0; 
        for (int p : pos[x]) {
            seg.apply(l, p + 1, p + 1); 
            l = p + 1; 
        }
        if (l < N) {
            seg.apply(l, N, N + 1); 
        }
        ans += base - seg.all_prod().sum; 
    }
    print(ans); 
}