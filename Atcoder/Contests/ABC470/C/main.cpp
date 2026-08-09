#include "library/template.hpp"

using namespace std;
using namespace suisen;
constexpr int LG = 20;

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, Q;
    read(N, Q);
    vector<int> pos(N), cnt(Q + 2);
    vector<vector<unsigned char>> par(LG);
    int mask[LG];
    REP(b, LG) {
        par[b].resize(1 << b);
        mask[b] = (1 << b) - 1;
    }
    auto toggle = [&](int x) {
        REP(b, LG) {
            par[b][x & mask[b]] ^= 1;
        }
    };
    int dec = 0;
    int ans = 0;
    REP(_, Q) {
        int t;
        read(t);
        if (t == 1) {
            int x;
            read(x);
            --x;
            int v = pos[x] > dec ? pos[x] - dec : 0;
            if (pos[x] > dec) {
                --cnt[pos[x]];
                toggle(pos[x]);
                ++pos[x];
            } else {
                pos[x] = dec + 1;
            }
            ++cnt[pos[x]];
            toggle(pos[x]);
            ans ^= v ^ (v + 1);
        } else {
            REP(b, LG) {
                if (par[b][dec & mask[b]]) {
                    ans ^= 1 << b;
                }
            }
            ++dec;
            if (cnt[dec] & 1) {
                toggle(dec);
            }
            cnt[dec] = 0;
        }
        print(ans);
    }
}