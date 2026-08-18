#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int N, M, Q; 
    read(N, M, Q); 
    vector<string> s(N); 
    read(s); 
    vector<int> a(N * M); 
    int cnt = 0; 
    REP(i, N) {
        REP(j, M) {
            if (s[i][j] == '*') {
                a[j * N + i] = 1; 
                ++cnt; 
            }
        }
    }
    FenwickTree<int> fw(a); 
    LOOP(Q) {
        int x, y; 
        read(x, y); 
        --x, --y; 
        int p = y * N + x; 
        int v = s[x][y] == '*' ? -1 : 1; 
        s[x][y] = v == 1 ? '*' : '.'; 
        cnt += v; 
        fw.add(p, v); 
        print(cnt - fw.sum(0, cnt)); 
    }
}