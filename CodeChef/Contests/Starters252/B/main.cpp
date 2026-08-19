#include "library/template.hpp"

using namespace std;
using namespace suisen;

signed main() {
    int T;
    read(T);

    LOOP(T) {
        int N;
        read(N);

        vector<int> A(2 * N);
        read(A);

        int ans = 0;

        REP(i, N) {
            ans += max(A[i], A[2 * N - 1 - i]);
        }

        print(ans);
    }
}