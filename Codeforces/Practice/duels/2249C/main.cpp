#include "library/template.hpp"
#include "library/datastructure/segment_tree/lazy_segment_tree.hpp"
using namespace std; 
using namespace suisen; 
using ll = long long; 
using ull = unsigned long long; 
int op(int x, int y) {
    return max(x, y); 
}
int e() {
    return -1000000000; 
}
int mapping(int f, int x) {
    return f + x; 
}
int composition(int f, int g) {
    return f + g; 
}
int id() {
    return 0; 
}
signed main() {
    int T; 
    read(T); 
    LOOP(T) {
        int N; 
        read(N); 
        vector<int> P(N), pos(N + 1); 
        read(P); 
        REP(i, N) {
            pos[P[i]] = i; 
        }
        vector<int> init(2 * N, e()); 
        vector<char> used(N + 1); 
        int blocks = 0; 
        REP(i, N) {
            int x = P[i]; 
            int d = 0; 
            if (x > 1) {
                d += used[x - 1]; 
            }
            if (x < N) {
                d += used[x + 1]; 
            }
            blocks += 1 - d; 
            used[x] = true; 
            init[i] = blocks; 
        }
        LazySegmentTree<int, op, e, int , mapping, composition, id> seg(init); 
        int ans =0; 
        REP(s, N) {
            if (seg.prod(s, s + N) <= 2) {
                ++ans; 
            }
            seg.apply(s + 1, s + N , -1);
            int x = P[s]; 
            if (x > 1) {
                int t = pos[x - 1]; 
                if (t < s) {
                    t += N; 
                }
                seg.apply(t, s + N, 1); 
            }
            if (x < N) {
            int t = pos[x + 1]; 
            if (t < s) {
                t += N; 
            }
            seg.apply(t, s + N, 1); 
            
        } 
        seg.set(s + N, 1); 
        }
        
        print(ans); 
    }
    
}
 