#include "library/template.hpp"
using namespace std;
using namespace suisen;
using ll = long long; 
constexpr int B = 9; 
signed main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(NULL); 
    int Q; 
    read(Q); 
    array<deque<ll>, 5> q; 
    array<int, 3> cnt{}; 
    auto merge_queue = [&](int src, int dst, vector<pair<int, int>>& ops) {
        int a= q[src].size(); 
        int b = q[dst].size(); 
        while (a || b) {
            if (b == 0 || (a && q[src].front() < q[dst].front())) {
                ll x = q[src].front(); 
                q[src].pop_front(); 
                q[dst].push_back(x); 
                ops.emplace_back(src, dst); 
                --a; 
            } else {
                ll x = q[dst].front(); 
                q[dst].pop_front(); 
                q[dst].push_back(x); 
                ops.emplace_back(dst, dst); 
                --b; 
            }
        }
    };  
    REP(_, 2 * Q) {
        int t;
        read(t); 
        if (t == -1) {
            return 0; 
        }
        if (t== 1) {
            ll X; 
            read(X); 
            q[0].push_back(X); 
            vector<pair<int, int>> ops; 
            merge_queue(0, 1, ops); 
            if (++cnt[0] == B) {
                cnt[0] = 0; 
                merge_queue(1, 2, ops); 
                if (++cnt[1] == B) {
                    cnt[1] = 0; 
                    merge_queue(2, 3, ops); 
                    if (++cnt[2] == B) {
                        cnt[2] = 0; 
                        merge_queue(3, 4, ops); 
                    }
                }
            }
            print(ops.size()); 
            FOR(op, ops) {
                print(op.first + 1, op.second + 1); 
            }
            cout.flush(); 
        } else {
            int p = -1; 
            REP(i, 1, 5) {
                if (q[i].empty()) {
                    continue; 
                }
                if (p == -1 || q[i].front() < q[p].front()) {
                    p = i; 
                }
            }
            print(p + 1); 
            cout.flush(); 
            q[p].pop_front(); 
        }
    }
}