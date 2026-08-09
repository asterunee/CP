#include "library/template.hpp"

using namespace std;
using namespace suisen;

struct Sq {
    int r, c, s;
};

void build(int n, int r0, int c0, vector<Sq>& ans) {
    if (n == 1) {
        return;
    }
    if (n == 3) {
        ans.push_back({ r0, c0, 1 });
        return;
    }
    build(n - 4, r0 + 2, c0 + 2, ans);

    ans.push_back({ r0, c0, n - 1 });

    REP(c, 3, n - 1, 2) {
        ans.push_back({ r0, c0 + c - 1, 1 });
    }
    REP(r, 3, n - 1, 2) {
        ans.push_back({ r0 + r - 1, c0 + n - 2, 1 });
    }
    REP(c, 2, n - 2, 2) {
        ans.push_back({ r0 + n - 2, c0 + c - 1, 1 });
    }
    REP(r, 2, n - 2, 2) {
        ans.push_back({ r0 + r - 1, c0, 1 });
    }
}

void solve() {
    int H, W;
    read(H, W);
    bool tr = H > W;
    if (tr) swap(H, W);
    vector<Sq> ans;
    if ((H & 1) and (W & 1)) {
        build(H, 1, 1, ans);
        REP(r, 1, H, 2) {
            REP(c, H + 1, W, 2) {
                ans.push_back({ r, c, 1 });
            }
        }
    } else {
        int h = H - (H & 1);
        int w = W - (W & 1);
        REP(r, 1, h, 2) {
            REP(c, 1, w, 2) {
                ans.push_back({ r, c, 1 });
            }
        }
    }
    if (tr) {
        for (auto& [r, c, s] : ans) {
            swap(r, c);
        }
    }
    print(ans.size());
    for (auto [r, c, s] : ans) {
        print(r, c, s);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    read(T);
    LOOP(T) solve();
    return 0;
}