#include "library/template.hpp"

using namespace std;
using namespace suisen;

signed main() {
    int T;
    read(T);

    LOOP(T) {
        int N;
        read(N);

        int o = 0, e = 0;

        LOOP(N) {
            int x;
            read(x);

            if (x & 1) ++o;
            else ++e;
        }

        print(2 * min(o, e) + (o != e));
    }
}