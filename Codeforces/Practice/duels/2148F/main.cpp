#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        int N; 
        read(N); 
        vector<vector<int>> A(N); 
        int L = 0; 
        REP(i, N) {
            int K; 
            read(K); 
            A[i].resize(K); 
            read(A[i]); 
            chmax(L, K); 
        }
        vector<vector<int>> col(L); 
        REP(i, N) {
            REP(j, (int)A[i].size()) {
                col[j].push_back(i); 
            }
        }
        vector<int> rk(N), best(L); 
        RREP(p, L) {
            vector<tuple<int, int, int>> v; 
            v.reserve(col[p].size()); 
            for (int i : col[p]) {
                v.emplace_back(A[i][p], rk[i], i); 
            }
            sort(ALL(v)); 
            best[p] = get<2>(v[0]); 
            int r = 0; 
            REP(j, (int) v.size()) {
                if (j and pair{get<0>(v[j - 1]), get<1>(v[j - 1])} != pair{get<0>(v[j]), get<1>(v[j])}) {
                    ++r; 
                }
                rk[get<2>(v[j])] = r; 
            }
        }
        vector<int> ans; 
        for (int p = 0; p < L; ){ 
            int i = best[p]; 
            for (int j = p; j < (int) A[i].size(); ++j) {
                ans.push_back(A[i][j]); 
            }
            p = A[i].size(); 
        }
        print(ans); 
    }
} 
