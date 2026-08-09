#include "library/template.hpp"
#include "library/datastructure/segment_tree/segment_tree.hpp"

using namespace std;
using namespace suisen;
constexpr int NEG =-1e9; 
struct Best {
    array<pair<int, int>, 3> a; 
};
void add(Best& x, int v, int c) {
    if (c < 0) {
        return; 
    }
    REP(i, 3) {
        if (x.a[i].second == c) {
            chmax(x.a[i].first, v); 
            sort(x.a.rbegin(), x.a.rend()); 
            return; 
        }
    }
    if (v > x.a[2].first) {
        x.a[2] = {v, c}; 
        sort(x.a.rbegin(), x.a.rend()); 
    }
}
Best op(Best x, Best y) {
    for (auto [v, c] : y.a) {
        add(x, v, c); 
    }
    return x; 
}
Best e() {
    return Best{{{{NEG, -1}, {NEG, -1}, {NEG, -1}}}}; 
}
int get(const Best& x, int c, int def) {
    for (auto [v, k] : x.a) {
        if (k >= 0 and k != c) {
            return v; 
        }
    }
    return def; 
}
using Seg = SegmentTree<Best, op, e>; 

signed main() {
    int T; 
    read(T); 
    while (T--) {
        int (N); 
        read(N); 
        vector<int> A(N); 
        read(A); 
        for (int& x : A) {
            --x; 
        }
        Best d0 = e(), d1 = e();
        vector<Best> base(N + 1, e()), ex(N, e());
        Seg seg(base);
        for (int y : A) {
            Best q = e();

            if (y) {
                q = op(q, seg(0, y));
            }
            if (y + 1 <= N) {
                q = op(q, seg(y + 1, N + 1));
            }
            q = op(q, ex[y]);
            Best nd1 = d1;
            int z = get(d1, y, NEG);
            if (z > NEG) {
                add(nd1, z + 1, y);
            }
            for (auto [v, x] : q.a) {
                if (x >= 0 and x != y) {
                    add(nd1, v + 1, x);
                }
            }
            if (d0.a[0].second < 0) {
                add(base[N], 1, y);
                seg.set(N, base[N]);
            } else {
                auto [v, p] = d0.a[0];
                int w = max(0, d0.a[1].first);

                add(base[p], v + 1, y);
                add(ex[p], w + 1, y);

                seg.set(p, base[p]);
            }
            add(d0, get(d0, y, 0) + 1, y);
            d1 = nd1;
        }
        print(max(d0.a[0].first, d1.a[0].first));
    } 
}