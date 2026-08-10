#include "library/template.hpp"
#include "library/number/sieve_of_eratosthenes.hpp"

using namespace std;
using namespace suisen;

SimpleSieve<200001> sieve;

signed main() {
    int T;
    read(T);

    while (T--) {
        int N;
        read(N);

        print(sieve.is_prime(N + 1) ? "YES" : "NO");
    }
}