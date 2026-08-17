#include "library/template.hpp"
#include "library/datastructure/segment_tree/segment_tree.hpp"
#include "library/linear_algebra/array_matrix.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
constexpr ll INF = 4e18; 
constexpr int BS = 64; 
ll mn(ll a, ll b) {
    return min(a, b); 
}
ll neg(ll a) {
    return a; 
}
ll inf() {
    return INF; 
}
ll add(ll a, ll b) {
    return min(INF, a + b); 
}
ll inv(ll a){
    return a; 
}
ll zero() {
    return 0 ;
}
using Mat = SquareArrayMatrix<ll, 5, mn, neg, inf, add, inv, zero>; 
Mat op(Mat a, Mat b) {
    return a * b; 
}
Mat e() {
    return Mat::e1(); 
}
int X; 
Mat step(Mat a, ll d, ll s) {
    Mat b = a; 
    REP(i, X) {
        array<ll, 5> pre, suf; 
        pre[0] = a[i][0]; 
        REP(j, 1, X) {
            pre[j] = min(pre[j - 1], a[i][j]); 
        }
        suf[X- 1] = a[i][X - 1]; 
        RREP(j, X - 1) {
            suf[j] = min(suf[j + 1], a[i][j]); 
        }
        REP(j, X) {
            ll y = d - X + 1 + j; 
            if (y <= 0) {
                b[i][j] = a[i][y +X  - 1]; 
            } else {
                ll t = y - 1; 
                ll q = t / X; 
                int r = t % X; 
                b[i][j] = suf[r] + q * s; 
                if (r) {
                    chmin(b[i][j], pre[r - 1] + (q + 1) * s); 
                }
            }
        }
    }
    return b; 
}
signed main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(NULL); 
    int N, Q; 
    read(N, Q, X); 
    vector<ll> d(N), s(N); 
    read(d, s); 
    int nb = (N + BS - 1) / BS; 
    auto build = [&](int b) {
        Mat a = e(); 
        REP(i, b * BS, min(N, (b + 1) * BS)) {
            a = step(a, d[i], s[i]); 
        }
        return a; 
    }; 
    vector<Mat> a(nb); 
    REP(i, nb) {
        a[i] = build(i); 
    }
    SegmentTree<Mat, op, e> seg(a); 
    LOOP(Q) {
        char t; 
        ll l, r; 
        read(t, l, r); 
        --l; 
        if (t != '?') {
            (t == '1' ? d[l] : s[l]) = r; 
            int b = l/ BS; 
            seg.set(b, build(b)); 
            continue; 
        }
        int R = r; 
        int bl = l / BS; 
        int br = (R -  1) / BS; 
        Mat a = e(); 
        if (bl == br) {
            REP(i, l, R) {
                a = step(a, d[i], s[i]); 
            }
        } else {
            REP(i, l, (bl + 1) * BS) {
                a = step(a, d[i], s[i]); 
            }
            if (bl + 1 < br) {
                a = op(a, seg.prod(bl + 1, br)); 
            }
            REP(i, br * BS, R) {
                a = step(a, d[i], s[i]); 
            }
        }
        print(a[0][X - 1]); 
    }
}