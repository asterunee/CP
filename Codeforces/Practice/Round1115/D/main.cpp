#include "library/template.hpp"

using namespace std;
using namespace suisen;
using ll = long long;

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    read(T);

    while (T--) {
        int N;
        read(N);
        vector<ll> A(N);
        read(A);
        vector<ll> D(N - 1);
        REP(i, N - 1) {
            D[i] = A[i + 1] - A[i];
        }
        for (int l = 0; l < N - 1;) {
            int r = l + 1;
            while (r < N - 1 and (D[r] & 1) == (D[l] & 1)) {
                ++r;
            }
            sort(D.begin() + l, D.begin() + r);
            l = r;
        }
        REP(i, N - 1) {
            A[i + 1] = A[i] + D[i];
        }
        print(A);
    }
}