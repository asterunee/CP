#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <climits>
#include <cstdlib>
using namespace std;

using ll = long long;

struct FastRNG {
    uint32_t x;
    explicit FastRNG(uint32_t seed) : x(seed ? seed : 1u) {}
    uint32_t next() {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return x;
    }
    int mod(int n) { return (int)(next() % (uint32_t)n); }
};

static void shuffle_ints(vector<int>& a, FastRNG& rng) {
    for (int i = (int)a.size() - 1; i > 0; --i) {
        int j = rng.mod(i + 1);
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

struct Board {
    int R = -1;
    vector<vector<string>> g;
};

struct SimResult {
    bool valid = false;
    ll cost = (1LL << 62);
    ll E = (1LL << 60);
    ll L = (1LL << 60);
    int D = INT_MAX;
    vector<ll> stored;
};

static string jump_token(int d, char dir) {
    if (d <= 0) return "X";
    if (d == 1) return string(1, dir);
    return to_string(d) + dir;
}

struct AssignmentK {
    vector<int> dst;
    ll err = (1LL << 60);
};

static AssignmentK evalK(const vector<ll>& A, const vector<ll>& B, const vector<int>& dst, int K) {
    int n = (int)A.size();
    vector<ll> got(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < K; ++k) got[dst[K*i+k]] += (A[i] + K - 1 - k) / K;
    }
    AssignmentK z; z.dst = dst; z.err = 0;
    for (int j = 0; j < n; ++j) z.err += llabs(got[j] - B[j]);
    return z;
}

static vector<AssignmentK> makeK(const vector<ll>& A, const vector<ll>& B, int K) {
    int n = (int)A.size(), N = K*n;
    vector<int> val(N);
    for (int i=0;i<n;++i) {
        for (int k=0;k<K;++k) { val[K*i+k]=(int)((A[i]+K-1-k)/K); }
    }
    FastRNG rng(712367u + n*991u + K*100003u);
    vector<AssignmentK> ans;
    ll global_best=(1LL<<60);
    for (int rep=0; rep<12; ++rep) {
        vector<int> chunks(N); for (int z=0; z<N; ++z) chunks[z]=z;
        shuffle_ints(chunks, rng);
        stable_sort(chunks.begin(),chunks.end(),[&](int x,int y){return val[x]>val[y];});
        vector<int> slot(N,-1), cnt(n,0); vector<ll> sum(n,0);
        for (int x:chunks) {
            int bj=-1; ll bs=-(1LL<<60);
            for(int j=0;j<n;++j) if(cnt[j]<K) {
                ll deficit=B[j]-sum[j];
                ll score=deficit*1000 - llabs((sum[j]+val[x])-B[j]) + (ll)(rng.next()%31);
                if(score>bs){bs=score;bj=j;}
            }
            int pos=K*bj+cnt[bj]++; slot[pos]=x; sum[bj]+=val[x];
        }
        ll cur=0; for(int j=0;j<n;++j) cur+=llabs(sum[j]-B[j]);
        vector<int> bestslot=slot; ll best=cur;
        for(int it=0;it<70000;++it){
            int p=(int)(rng.next()%N),q=(int)(rng.next()%N); if(p==q||p/K==q/K) continue;
            int x=slot[p],y=slot[q],a=p/K,b=q/K;
            ll old=llabs(sum[a]-B[a])+llabs(sum[b]-B[b]);
            sum[a]+=val[y]-val[x]; sum[b]+=val[x]-val[y];
            ll nw=llabs(sum[a]-B[a])+llabs(sum[b]-B[b]);
            bool ac=nw<=old;
            if(!ac && (it%4000)<40) ac=true;
            if(ac){swap(slot[p],slot[q]);cur+=nw-old;if(cur<best){best=cur;bestslot=slot;}}
            else {sum[a]-=val[y]-val[x];sum[b]-=val[x]-val[y];}
        }
        vector<int> dst(N);
        for(int p=0;p<N;++p) dst[bestslot[p]]=p/K;
        AssignmentK z=evalK(A,B,dst,K);
        bool dup=false;for(auto &w:ans) if(w.dst==z.dst) dup=true;
        if(!dup){ans.push_back(z);sort(ans.begin(),ans.end(),[](const AssignmentK&x,const AssignmentK&y){return x.err<y.err;});if(ans.size()>6)ans.resize(6);}
        global_best=min(global_best,z.err);
        if(global_best==0 && rep>=3) break;
    }
    return ans;
}

static bool buildK(const vector<ll>& A, const vector<int>& dst, int M, int R, int K, uint32_t seed, Board& out) {
    int n=(int)A.size();
    FastRNG rng(seed);
    vector<int> ord; for(int i=0;i<n;++i)if(A[i]>0)ord.push_back(i);
    shuffle_ints(ord, rng);
    vector<vector<unsigned char>> occ(R,vector<unsigned char>(n,0));
    vector<vector<string>> g(R,vector<string>(n,"X"));
    for(int i:ord) occ[0][i]=1;
    vector<int> rmask(n,0);
    auto token=[&](int r1,int c1,int r2,int c2)->string{
        if(r1==r2){int d=abs(c2-c1);return jump_token(d,c2>c1?'R':'L');}
        int d=abs(r2-r1);return jump_token(d,r2>r1?'D':'U');
    };
    for(int ii=0;ii<(int)ord.size();++ii){
        int i=ord[ii];
        struct Cand{int score;vector<vector<unsigned char>> o;vector<vector<string>> gg;vector<int> rm;};
        vector<Cand> cs;
        for(int y=2;y<=R-1;++y){
            vector<char> avail={'U','D'}; if(i>0)avail.push_back('L'); if(i+1<n)avail.push_back('R');
            if((int)avail.size()<K)continue;
            vector<int> choose(avail.size(),0); fill(choose.begin(),choose.begin()+K,1);
            sort(choose.begin(),choose.end(),greater<int>());
            do{
                vector<char> dirs;for(int z=0;z<(int)avail.size();++z)if(choose[z])dirs.push_back(avail[z]);
                sort(dirs.begin(),dirs.end());
                do{
                    auto o=occ;auto gg=g;auto rm=rmask;
                    if(o[y-1][i])continue;
                    o[y-1][i]=1;
                    string st; bool ok=true;int score=0;
                    for(int k=0;k<K&&ok;++k){
                        char d=dirs[k];st.push_back(d);
                        ll amount=(A[i]+K-1-k)/K; if(amount==0)continue;
                        int lr=y,lc=i;
                        if(d=='U')--lr;else if(d=='D')++lr;else if(d=='L')--lc;else ++lc;
                        if(lr<1||lr>R||lc<0||lc>=n||o[lr-1][lc]){ok=false;break;}
                        int b=dst[K*i+k]; int fr=lr;
                        vector<pair<int,int>> cells; cells.push_back({lr,lc});
                        if(lc!=b){if(o[lr-1][b]){ok=false;break;}cells.push_back({lr,b});}
                        int base=(int)(((M-A[i]+1)+k+2+(lc!=b))%K);
                        int bestd=-1,bestres=-1;vector<int> bestq;
                        for(int want=0;want<K;++want)if(!(rm[b]&(1<<want))){
                            int dd=(want-base+K)%K;vector<int> freeRows;
                            for(int rr=1;rr<=R;++rr){
                                bool used=o[rr-1][b];
                                for(auto cc:cells)if(cc.first==rr&&cc.second==b)used=true;
                                if(!used)freeRows.push_back(rr);
                            }
                            if((int)freeRows.size()<dd)continue;
                            vector<int> qs;
                            if(dd){
                                if(rng.next()&1) reverse(freeRows.begin(),freeRows.end());
                                for(int z=0;z<dd;++z)qs.push_back(freeRows[z]);
                            }
                            if(bestd<0||dd<bestd){bestd=dd;bestres=want;bestq=qs;}
                        }
                        if(bestd<0){ok=false;break;}
                        for(auto cc:cells)o[cc.first-1][cc.second]=1;
                        if(lc!=b)gg[lr-1][lc]=token(lr,lc,lr,b);
                        int cr=fr,cc=b;
                        for(int q:bestq){gg[cr-1][cc]=token(cr,cc,q,b);o[q-1][b]=1;cr=q;}
                        gg[cr-1][b]=token(cr,b,R+1,b);
                        rm[b]|=1<<bestres;
                        score+=bestd*20+(lc!=b)*3+abs(lc-b);
                    }
                    if(!ok)continue;
                    gg[y-1][i]=st;
                    gg[0][i]=token(1,i,y,i);
                    score+=y;
                    cs.push_back({score,move(o),move(gg),move(rm)});
                }while(next_permutation(dirs.begin(),dirs.end()));
            }while(prev_permutation(choose.begin(),choose.end()));
        }
        if(cs.empty())return false;
        sort(cs.begin(),cs.end(),[](const Cand&a,const Cand&b){return a.score<b.score;});
        int lim=min(6,(int)cs.size());int pick=(int)(rng.next()%lim);
        occ=move(cs[pick].o);g=move(cs[pick].gg);rmask=move(cs[pick].rm);
    }
    out.R=R;out.g=move(g);return true;
}


static vector<int> best_permutation(const vector<ll>& A, const vector<ll>& B) {
    const int n = (int)A.size();
    const int S = 1 << n;
    const int INF = 1000000000;
    vector<int> dp(S, INF), parent(S, -1);
    dp[0] = 0;
    for (int mask = 0; mask < S; ++mask) {
        if (dp[mask] == INF) continue;
        int i = __builtin_popcount((unsigned)mask);
        if (i == n) continue;
        for (int j = 0; j < n; ++j) if (!(mask & (1 << j))) {
            int nm = mask | (1 << j);
            int nv = dp[mask] + (int)llabs(A[i] - B[j]);
            if (nv < dp[nm]) {
                dp[nm] = nv;
                parent[nm] = j;
            }
        }
    }
    vector<int> to(n, 0);
    int mask = S - 1;
    for (int i = n - 1; i >= 0; --i) {
        int j = parent[mask];
        if (j < 0) j = i;
        to[i] = j;
        mask ^= 1 << j;
    }
    return to;
}

static bool build_permutation_board(const vector<ll>& A, const vector<int>& to,
                                    int R, uint32_t seed, Board& out) {
    const int n = (int)A.size();
    FastRNG rng(seed);
    vector<int> order;
    for (int i = 0; i < n; ++i) if (A[i] > 0 && to[i] != i) order.push_back(i);
    shuffle_ints(order, rng);

    vector<vector<unsigned char>> used(R, vector<unsigned char>(n, 0));
    vector<vector<string>> g(R, vector<string>(n, "X"));

    for (int i = 0; i < n; ++i) {
        if (A[i] == 0) continue;
        used[0][i] = 1;
        if (to[i] == i) g[0][i] = jump_token(R, 'D');
    }

    for (int i : order) {
        int b = to[i];
        vector<int> rows;
        for (int r = 2; r <= R; ++r) {
            if (!used[r - 1][i] && !used[r - 1][b]) rows.push_back(r);
        }
        if (rows.empty()) return false;
        int r = rows[rng.mod(min(3, (int)rows.size()))];
        used[r - 1][i] = used[r - 1][b] = 1;
        g[0][i] = jump_token(r - 1, 'D');
        g[r - 1][i] = jump_token(abs(b - i), b > i ? 'R' : 'L');
        g[r - 1][b] = jump_token(R + 1 - r, 'D');
    }

    out.R = R;
    out.g = move(g);
    return true;
}

static Board direct_board(int n) {
    Board b;
    b.R = n;
    b.g.assign(n, vector<string>(n, "X"));
    for (int c = 0; c < n; ++c) b.g[0][c] = jump_token(n, 'D');
    return b;
}

struct ParsedCell {
    int k = 0;
    int dr[4] = {0,0,0,0};
    int dc[4] = {0,0,0,0};
    int len[4] = {0,0,0,0};
};

static SimResult simulate_board(const vector<ll>& A, const vector<ll>& B,
                                int T, int M, const Board& board) {
    const int C = (int)A.size();
    const int R = board.R;
    SimResult res;
    if (R < C || R > C + 20 || (int)board.g.size() != R) return res;

    const int H = R + 1; 
    vector<ParsedCell> cell(R * C);

    auto dir_id = [](char ch) -> pair<int,int> {
        if (ch == 'U') return {-1, 0};
        if (ch == 'D') return {1, 0};
        if (ch == 'L') return {0, -1};
        return {0, 1};
    };

    for (int r = 0; r < R; ++r) {
        if ((int)board.g[r].size() != C) return res;
        for (int c = 0; c < C; ++c) {
            const string& s = board.g[r][c];
            ParsedCell pc;
            if (s == "X") {
                pc.k = 0;
            } else if (isdigit((unsigned char)s[0])) {
                int p = 0, d = 0;
                while (p < (int)s.size() && isdigit((unsigned char)s[p])) {
                    d = d * 10 + (s[p] - '0');
                    ++p;
                }
                if (d <= 0 || p + 1 != (int)s.size()) return res;
                auto [x, y] = dir_id(s[p]);
                pc.k = 1; pc.dr[0] = x; pc.dc[0] = y; pc.len[0] = d;
            } else {
                bool seen[4] = {};
                pc.k = (int)s.size();
                if (pc.k < 1 || pc.k > 4) return res;
                for (int z = 0; z < pc.k; ++z) {
                    char ch = s[z];
                    int id = (ch == 'U' ? 0 : ch == 'D' ? 1 : ch == 'L' ? 2 : ch == 'R' ? 3 : -1);
                    if (id < 0 || seen[id]) return res;
                    seen[id] = true;
                    auto [x, y] = dir_id(ch);
                    pc.dr[z] = x; pc.dc[z] = y; pc.len[z] = 1;
                }
            }
            cell[r * C + c] = pc;
        }
    }

    vector<int> q(H * C, 0), ptr(R * C, 0);
    vector<unsigned char> over(H * C, 0);
    vector<int> from, to;
    from.reserve(R * C * 4);
    to.reserve(R * C * 4);
    vector<ll> stored(C, 0);
    int last = 0;

    for (int t = 1; t <= T; ++t) {
        for (int c = 0; c < C; ++c) {
            if (t >= M - A[c] + 1 && t <= M) ++q[c];
        }

        fill(over.begin(), over.end(), 0);
        from.clear(); to.clear();

        for (int r = 0; r < R; ++r) {
            for (int c = 0; c < C; ++c) {
                const int id = r * C + c;
                const int have = q[id];
                if (have == 0) continue;
                const ParsedCell& pc = cell[id];
                if (pc.k == 0) {
                    over[id] = 1;
                    continue;
                }
                const int send = min(have, pc.k);
                if (have > pc.k) over[id] = 1;
                q[id] -= send;
                for (int z = 0; z < send; ++z) {
                    const int e = (ptr[id] + z) % pc.k;
                    const int nr = r + pc.dr[e] * pc.len[e];
                    const int nc = c + pc.dc[e] * pc.len[e];
                    if (nr < 0 || nr > R || nc < 0 || nc >= C) return res;
                    from.push_back(id);
                    to.push_back(nr * C + nc);
                }
                ptr[id] = (ptr[id] + send) % pc.k;
            }
        }

        for (int c = 0; c < C; ++c) {
            const int id = R * C + c;
            if (q[id] > 0) over[id] = 1;
        }

        for (size_t z = 0; z < to.size(); ++z) {
            if (over[to[z]]) ++q[from[z]];
            else ++q[to[z]];
        }

        for (int c = 0; c < C; ++c) {
            const int id = R * C + c;
            if (q[id] > 0) {
                --q[id];
                ++stored[c];
                last = t;
            }
        }
    }

    ll E = 0;
    for (int c = 0; c < C; ++c) E += llabs(stored[c] - B[c]);
    ll need = 0, done = 0;
    for (int c = 0; c < C; ++c) { need += B[c]; done += stored[c]; }
    const ll L = need - done;
    const int D = (L > 0 ? T : last - M);
    const ll row_cost = 1LL << (R - C);

    res.valid = true;
    res.E = E;
    res.L = L;
    res.D = D;
    res.cost = row_cost + max<ll>(E, D) + (ll)T * L;
    res.stored = move(stored);
    return res;
}

static bool better_result(const SimResult& x, const Board& bx,
                          const SimResult& y, const Board& by) {
    if (!x.valid) return false;
    if (!y.valid) return true;
    if (x.cost != y.cost) return x.cost < y.cost;
    if (x.L != y.L) return x.L < y.L;
    if (x.E != y.E) return x.E < y.E;
    if (x.D != y.D) return x.D < y.D;
    return bx.R < by.R;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C, T, M;
    if (!(cin >> C >> T >> M)) return 0;
    vector<ll> A(C), B(C);
    for (ll& x : A) cin >> x;
    for (ll& x : B) cin >> x;

    Board best_board = direct_board(C);
    SimResult best_result = simulate_board(A, B, T, M, best_board);

    uint32_t base_seed = 0x9E3779B9u;
    for (ll x : A) base_seed = base_seed * 1664525u + (uint32_t)x + 1013904223u;
    for (ll x : B) base_seed = base_seed * 1664525u + (uint32_t)x + 1013904223u;

    vector<int> perm_to = best_permutation(A, B);
    for (int rr = C; rr <= min(C + 2, C + 20); ++rr) {
        for (int v = 0; v < 40; ++v) {
            Board bb;
            if (!build_permutation_board(A, perm_to, rr,
                    base_seed ^ (uint32_t)(0xA17E000u + rr * 131 + v), bb)) continue;
            SimResult cur = simulate_board(A, B, T, M, bb);
            if (better_result(cur, bb, best_result, best_board)) {
                best_result = move(cur);
                best_board = move(bb);
            }
        }
    }

    const int degree_order[2] = {3, 2};
    for (int phase = 0; phase < 2; ++phase) {
        int K = degree_order[phase];
        vector<AssignmentK> cand = makeK(A, B, K);
        if (cand.empty()) continue;

        if (1 + cand[0].err >= best_result.cost) continue;

        int keep = (K == 2 ? 4 : 5);
        int variants = (K == 2 ? 28 : 48);
        int extra = (K == 2 ? 3 : 4);
        for (int ai=0; ai<(int)cand.size() && ai<keep; ++ai) {
            if (1 + cand[ai].err >= best_result.cost) break;
            for (int rr=C; rr<=min(C+extra,C+20); ++rr) {
                ll row_lower_bound = 1LL << (rr - C);
                if (row_lower_bound + cand[ai].err >= best_result.cost) continue;
                for (int v=0; v<variants; ++v) {
                    Board bb;
                    uint32_t sd = base_seed ^ (uint32_t)(K*0x51ED000u+ai*10007+rr*257+v);
                    if (!buildK(A,cand[ai].dst,M,rr,K,sd,bb)) continue;
                    SimResult cur=simulate_board(A,B,T,M,bb);
                    if (better_result(cur,bb,best_result,best_board)) {
                        best_result=move(cur);
                        best_board=move(bb);
                    }
                }
            }
        }
    }
    cout << best_board.R << '\n';
    for (int r = 0; r < best_board.R; ++r) {
        for (int c = 0; c < C; ++c) {
            if (c) cout << ' ';
            cout << best_board.g[r][c];
        }
        cout << '\n';
    }
    return 0;
}
