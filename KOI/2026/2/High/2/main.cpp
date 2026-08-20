#include "library/template.hpp"
#include "library/datastructure/fenwick_tree/fenwick_tree.hpp"
using namespace std; 
using namespace suisen; 
constexpr int INF = 1000000000; 
constexpr int LOG = 20; 
signed main() {
    int N, Q; 
    read(N, Q); 
    vector<int> A(N + 2); 
    REP(i, 1, N + 1) {
        read(A[i]); 
    }
    vector<array<int, 3>> qry(Q); 
    REP(i, Q) {
        read(qry[i][0], qry[i][1], qry[i][2]); 
    }
    vector<int> L(N + 2), R(N + 2); 
    vector<int> st; 
    st.reserve(N); 
    REP(i, 1, N + 1) {
        while (!st.empty() and A[st.back()] < A[i]) {
            st.pop_back(); 
        }
        L[i] = st.empty() ? 0 : st.back(); 
        st.push_back(i); 
    }
    st.clear(); 
    RREP(i, 1, N + 1) {
        while (!st.empty() and A[st.back()] < A[i]) {
            st.pop_back(); 
        }
        R[i] = st.empty() ? N + 1 : st.back(); 
        st.push_back(i); 
    }
    vector<array<int, LOG>> jl(N + 2), jr(N + 2); 
    REP(i, 1, N + 1) {
        jl[i][0] = L[i]; 
        jr[i][0] = R[i]; 
    }
    jr[N + 1][0]= N + 1; 
    REP(k, 1, LOG) {
        REP(i, 1, N + 1) {
            jl[i][k] = jl[jl[i][k - 1]][k - 1]; 
        }
        REP(i, 1, N + 2) {
            jr[i][k] = jr[jr[i][k - 1]][k - 1]; 
        }
    }
    vector<int> ord(N); 
    iota(ALL(ord), 1); 
    sort(ALL(ord), [&](int i, int j) {return A[i] < A[j]; });
    vector<int> D(N + 2, 1); 
    for (int x : ord) {
        if (L[x] == 0 or R[x] == N + 1) {
            D[x] = INF; 
        }
        if (L[x]) {
            chmax(D[L[x]], D[x] + 1); 
        }
        if (R[x] != N + 1) {
            chmax(D[R[x]], D[x] + 1); 
        }
    } 
    vector<array<int, 5>> ev; 
    ev.reserve(N + Q); 
    REP(i, 1, N + 1) {
        ev.push_back({D[i], 0, i, 0, 0}); 
    }
    REP(i, Q) {
        auto [l, r, t] = qry[i]; 
        ev.push_back({t, 1, l, r, i}); 
    }
    sort(ALL(ev)); 
    FenwickTree<int> fw(N); 
    vector<int> ans(Q); 
    for (auto e : ev) {
        auto [t, type, x, y, id] = e; 
        if (type == 0) {
            fw.add(x - 1, 1); 
            continue; 
        }
        int l =x,r = y; 
        int rem = fw.sum(l - 1, r); 
        int cl = l; 
        if (D[l] <= t) {
            --rem; 
            RREP(k, LOG) {
                int v = jr[cl][k];
                if (v <= r and D[v] <= t) {
                    cl = v; 
                    rem -= 1 << k; 
                }
            }
        }
        int cr= r; 
        if (D[r] <= t) {
            --rem; 
            RREP(k, LOG ){
                int v = jl[cr][k]; 
                if (v >= l and D[v] <= t) {
                    cr =v; 
                    rem -= 1 << k; 
                }
            }
        }
        if (cl == cr and D[cl] <= t) {
            ++rem; 
        }
        ans[id] = r - l + 1 - rem; 
    }
    for (int x : ans) {
        print(x); 

    }
}