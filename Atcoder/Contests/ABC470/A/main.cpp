#include "library/template.hpp"

using namespace std;
using namespace suisen;

signed main() {
    int N;
    read(N);

    REP(i, 1, N + 1) {
        if (i % 3 == 0) {
            print("Fizz");
        } else {
            print(i);
        }
    }
}