#include "library/template.hpp"
using namespace std; 
using namespace suisen; 

signed main() {
    int N; 
    string S; 
    read(N, S); 
    vector<int> pi(N); 
    REP(i, 1, N) {
        int j = pi[i - 1]; 
        while (j && S[i] != S[j]) {
            j = pi[j - 1]; 
        }
        if (S[i] == S[j]) {
            ++j; 
        }
        pi[i] = j; 
    }

    int p = N - pi[N - 1]; 
    if (N % p) {
        p = N; 
    }

    string T = S.substr(0, p); 
    int K = N / p; 

    if (p == 1) {
        print(K); 
        print(0); 
        print(""); 
        return 0; 
    }
    vector<vector<int>> occ(26); 
    REP(i, p) {
        occ[T[i] - 'a'].push_back(i); 
    }
    vector<int> ch; 
    REP(c, 26) {
        if (occ[c].size()) {
            ch.push_back(c); 
        }
    }
    vector<array<int, 26>> ps(p + 1); 
    REP(i, p) {
        ps[i + 1] = ps[i]; 
        ++ps[i + 1][T[i] - 'a']; 
    }
    vector<array<int, 26>> nx(p), pv(p); 
    vector<int> prv(p, -1); 
    REP(i, p) {
        REP(c, 26) {
            nx[i][c] = pv[i][c] = -1; 
        }
    }

    for (int c : ch) {
        auto& v = occ[c]; 
        int m = v.size(); 
        REP(i, m) {
            prv[v[i]] = v[(i + m - 1) % m]; 
        }
        REP(i, p) {
            auto it = lower_bound(ALL(v), i); 
            nx[i][c] = it == v.end() ? v[0] : *it; 
            it = upper_bound(ALL(v), i); 
            pv[i][c] = it == v.begin() ? v.back() : *prev(it); 
        }
    }

    auto cnt = [&](int c, int l, int len) {
        int r = l + len; 
        if (r <= p) {
            return ps[r][c] - ps[l][c]; 
        }
        return ps[p][c] - ps[l][c] + ps[r - p][c]; 
    };
    int F = p * p; 
    vector<int> par(F + 1, -1); 
    vector<char> how(F + 1); 
    queue<int> q; 
    REP(l, p) {
        int id = l * p; 
        par[id] = -2; 
        q.push(id); 
    }
    while (!q.empty() && par[F] == -1) {
        int id = q.front(); 
        q.pop(); 

        int l = id / p; 
        int len = id % p + 1; 
        int r = (l + len - 1) % p; 

        for (int c : ch) {
            int z = cnt(c, l, len); 
            if (!z) {
                continue; 
            }
            int to; 
            if (z == (int) occ[c].size()) {
                to = F; 
            } else {
                int a = nx[l][c]; 
                int b = pv[r][c]; 
                int nl = prv[a]; 
                int sz = (b - nl + p) % p; 
                to = nl * p + sz - 1; 
            }
            if (par[to] != -1) {
                continue; 
            }
            par[to] = id; 
            how[to] = 'a' + c; 
            q.push(to); 
        }
    }
    string X; 
    for (int v = F; par[v] != -2; v = par[v]) {
        X += how[v]; 
    }
    print(K); 
    print(X.size()); 
    print(X); 
}