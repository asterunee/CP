#include "library/template.hpp"
#include "library/number/kth_root_round.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        ll N, X; 
        read(N, X); 
        vector<vector<ll>> A(N), B(N); 
        vector<vector<tuple<ll, ll, int>>> seg(N); 
        REP(i, N) {
            int M; 
            read(M); 
            A[i].resize(M);
            B[i].resize(M);
            read(A[i]); 
            read(B[i]); 
            ll sum = 0, need = 0; 
            REP(j, M) {
                chmax(need, A[i][j] -sum); 
                sum += B[i][j] - A[i][j]; 
                if (sum > 0) {
                    seg[i].emplace_back(need, sum, j + 1); 
                    sum = 0; 
                    need = 0; 
                }
            }
        }
        vector<int> ptr(N), pos(N); 
        min_priority_queue<pair<ll, int>> pq; 
        REP(i, N) {
            if (!seg[i].empty()) {
                pq.emplace(get<0>(seg[i][0]), i); 
            }
        }
        while (!pq.empty() && pq.top().first <= X) {
            auto [_, i] = pq.top(); 
            pq.pop(); 
            auto [need, gain, r] = seg[i][ptr[i]]; 
            X += gain; 
            pos[i] = r; 
            ++ptr[i]; 
            if (ptr[i] < (int)seg[i].size()) {
                pq.emplace(get<0>(seg[i][ptr[i]]), i); 
            }
        }
        int best = -1, id = -1; 
        REP(i, N) {
            ll cur = X, h =pos[i]; 
            while (h < (int)A[i].size() && A[i][h] <= cur) {
                cur += B[i][h] - A[i][h]; 
                ++h; 
            }
            if (h > best) {
                best = h; 
                id = i; 
            }
        }
        print(best, id + 1); 
        
    }
}