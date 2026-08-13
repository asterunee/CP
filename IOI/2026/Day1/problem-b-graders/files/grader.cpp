#include "monuments.h"

#include <cassert>
#include <cstdio>
#include <vector>

int main() {
    int n, m;
    assert(std::scanf("%d%d", &n, &m) == 2);

    std::vector<int> coordinates(n);
    for (int& coordinate : coordinates) {
        assert(std::scanf("%d", &coordinate) == 1);
    }

    std::vector<int> ancient(m);
    for (int& index : ancient) {
        assert(std::scanf("%d", &index) == 1);
    }
    std::fclose(stdin);

    const long long result = get_cost(coordinates, ancient);
    std::printf("%lld\n", result);
    std::fclose(stdout);
    return 0;
}
