#include "library/template.hpp"
#include "library/datastructure/segment_tree/lazy_segment_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
constexpr int INF = 1 << 29; 
struct S {
    int x, i; 
}; 
S op(S a, S b) {
    if (a.x != b.x) {
        return a.x < b.x ? a : b; 
    }
    return a.i > b.i ? a : b; 
}
S e() {
    return {INF, -1}; 
}
S mapping(int f, S a) {
    return {a.x + f, a.i}; 
}
int composition(int f, int g) {
    return f + g; 
}
int id() {
    return 0; 
}
signed main() {
    int N; 
    read(N); 
    vector<int> A(N), cnt(N); 
    read(A); 
    for (int x : A) {
        ++cnt[x]; 
    }
    vector<S> init(N); 
    REP(x, N) {
        init[x] = cnt[x] ? S{x, x} : e(); 
    }
    LazySegmentTree<S, op, e, int, mapping, composition, id> seg(init); 
    vector<int> B; 
    B.reserve(N); 
    REP(_, N) {
        auto [d, x] = seg.all_prod(); 
        if (d != 0) {
            print("No"); 
            return 0; 
        }
        B.push_back(x); 
        --cnt[x]; 
        seg.apply(x + 1, N , -1); 
        if (cnt[x] == 0) {
            seg.set(x, e()); 
        }
    }
    print("Yes"); 
    print(B); 
}