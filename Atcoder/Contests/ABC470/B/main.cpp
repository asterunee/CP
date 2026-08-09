#include "library/template.hpp"

using namespace std;
using namespace suisen;

signed main() {
    int N;
    read(N);
    vector<int> C(N); 
    read(C); 
    vector<int> cnt(N + 1); 
    for (int c : C) {
        ++cnt[c]; 
    }
    print(N - *max_element(ALL(cnt))); 
}