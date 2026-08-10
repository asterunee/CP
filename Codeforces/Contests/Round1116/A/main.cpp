#include "library/template.hpp"

using namespace std;
using namespace suisen;
using ll = long long;

void solve() {
    ll a, b, c;
    read(a, b, c);
    array<ll, 3> v{ a, b, c };
    sort(v.begin(), v.end());
    print(min(v[2] - v[0], v[1]));
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    read(T);
    LOOP(T) {
        solve();
    }
    return 0;
}