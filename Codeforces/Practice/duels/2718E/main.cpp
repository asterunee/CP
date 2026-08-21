#include <bits/stdc++.h>
using namespace std;

using ll = long long;

ll ask(int l, int r) {
    cout << "? " << l << ' ' << r << endl;
    ll x;
    cin >> x;
    if (x == -1) exit(0);
    return x;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        int N;
        cin >> N;
        int l = 1, r = N;
        ll sum = ask(1, N);
        while (l < r) {
            sum /= 2;
            int lo = l, hi = r - 1;
            while (lo <= hi) {
                int mid = (lo + hi) / 2;
                if (ask(l, mid) < sum) {
                    lo = mid + 1;
                } else {
                    hi = mid - 1;
                }
            }
            int m = lo;
            if (m - l + 1 <= r - m) {
                r = m;
            } else {
                l = m + 1;
            }
        }
        cout << "! " << sum << endl;
    }
}