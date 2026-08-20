#include <bits/stdc++.h>
using namespace std;
using ll=long long;
using ld=long double;
using vi=vector<int>;
using vl=vector<ll>;
using pii=pair<int,int>;
using pll=pair<ll,ll>;
template<class T>using pqg=priority_queue<T,vector<T>,greater<T>>;
#define all(x) begin(x),end(x)
#define rall(x) rbegin(x),rend(x)
#define sz(x) (int)x.size()
#define rep(i,a,b) for(int i=a;i<b;i++)
#define per(i,a,b) for(int i=a;i>=b;i--)
const ll INF=4e18;
const int MOD=1e9+7;
const int MOD2=998244353;
ll modpow(ll a,ll b,ll m=MOD){
    ll r=1;
    while(b){
        if(b&1)r=r*a%m;
        a=a*a%m;
        b>>=1;
    }
    return r;
}
struct DSU{
    vi p,s;
    DSU(int n){p.resize(n);s.assign(n,1);iota(all(p),0);}
    int find(int x){return p[x]==x?x:p[x]=find(p[x]);}
    bool join(int a,int b){a=find(a);b=find(b);if(a==b)return 0;if(s[a]<s[b])swap(a,b);p[b]=a;s[a]+=s[b];return 1;}
};
struct BIT{
    int n;vl b;
    BIT(int n):n(n),b(n+1){}
    void add(int i,ll v){for(;i<=n;i+=i&-i)b[i]+=v;}
    ll sum(int i){ll r=0;for(;i;i-=i&-i)r+=b[i];return r;}
};
struct LazySeg{
    int n;vl t,lz;
    LazySeg(int n):n(n),t(4*n),lz(4*n){}
    void push(int x,int l,int r){
        if(!lz[x])return;
        int m=(l+r)/2;
        t[x*2]+=(m-l+1)*lz[x];
        t[x*2+1]+=(r-m)*lz[x];
        lz[x*2]+=lz[x];
        lz[x*2+1]+=lz[x];
        lz[x]=0;
    }
    void add(int x,int l,int r,int a,int b,ll v){
        if(b<l||r<a)return;
        if(a<=l&&r<=b){t[x]+=(r-l+1)*v;lz[x]+=v;return;}
        push(x,l,r);
        int m=(l+r)/2;
        add(x*2,l,m,a,b,v);
        add(x*2+1,m+1,r,a,b,v);
        t[x]=t[x*2]+t[x*2+1];
    }
};
struct Dinic{
    struct E{int to,rev;ll cap;};
    int n;vector<vector<E>>g;vi lv,it;
    Dinic(int n):n(n),g(n),lv(n),it(n){}
    void add(int a,int b,ll c){
        g[a].push_back({b,(int)g[b].size(),c});
        g[b].push_back({a,(int)g[a].size()-1,0});
    }
    bool bfs(int s,int t){
        fill(all(lv),-1);
        queue<int>q;q.push(s);lv[s]=0;
        while(q.size()){
            int x=q.front();q.pop();
            for(auto&e:g[x])if(e.cap&&lv[e.to]<0)lv[e.to]=lv[x]+1,q.push(e.to);
        }
        return lv[t]>=0;
    }
    ll dfs(int x,int t,ll f){
        if(x==t)return f;
        for(int&i=it[x];i<sz(g[x]);i++){
            auto&e=g[x][i];
            if(e.cap&&lv[e.to]==lv[x]+1){
                ll r=dfs(e.to,t,min(f,e.cap));
                if(r){e.cap-=r;g[e.to][e.rev].cap+=r;return r;}
            }
        }
        return 0;
    }
    ll flow(int s,int t){
        ll r=0;
        while(bfs(s,t)){
            fill(all(it),0);
            while(ll f=dfs(s,t,INF))r+=f;
        }
        return r;
    }
};
vector<int> prefix(string s){
    vector<int>p(sz(s));
    rep(i,1,sz(s)){
        int j=p[i-1];
        while(j&&s[i]!=s[j])j=p[j-1];
        if(s[i]==s[j])j++;
        p[i]=j;
    }
    return p;
}
vector<int> zfunc(string s){
    int n=sz(s),l=0,r=0;
    vector<int>z(n);
    rep(i,1,n){
        if(i<=r)z[i]=min(r-i+1,z[i-l]);
        while(i+z[i]<n&&s[z[i]]==s[i+z[i]])z[i]++;
        if(i+z[i]-1>r)l=i,r=i+z[i]-1;
    }
    return z;
}
struct SAM{
    struct Node{int nxt[26],link,len;Node(){memset(nxt,-1,sizeof(nxt));link=-1;len=0;}};
    vector<Node>st;int last;
    SAM(){st.push_back(Node());last=0;}
    void add(char c){
        int cur=sz(st);st.push_back(Node());st[cur].len=st[last].len+1;
        int p=last;
        while(p!=-1&&st[p].nxt[c-'a']==-1){st[p].nxt[c-'a']=cur;p=st[p].link;}
        if(p==-1)st[cur].link=0;
        else st[cur].link=st[p].nxt[c-'a'];
        last=cur;
    }
};
struct Point{
    ld x,y;
    Point(){}
    Point(ld x,ld y):x(x),y(y){}
    Point operator-(Point p){return {x-p.x,y-p.y};}
    ld cross(Point p){return x*p.y-y*p.x;}
};
int C, T_problem;
ll M;
vl A, B;
int R_grid;

ll evaluate(const vi& row_bias, const vector<vi>& choices) {
    vector<vl> down(R_grid + 2, vl(C, 0));
    rep(c, 0, C) down[1][c] = A[c];

    ll penalty = 0;

    rep(r, 1, R_grid + 1) {
        if (row_bias[r] == 0) {
            vl right(C + 1, 0);
            rep(c, 0, C) {
                ll in_top = down[r][c];
                ll in_left = right[c];
                ll tot = in_top + in_left;
                if (tot == 0) continue;

                int choice = choices[r][c];
                if (c == C - 1) {
                    if (choice != 0) {
                        penalty += 1e12;
                        choice = 0;
                    }
                }

                if (in_top > 0 && in_left > 0) {
                    if (choice != 2) {
                        penalty += 1e9;
                    }
                }

                if (choice == 0) {
                    down[r + 1][c] += tot;
                } else if (choice == 1) {
                    right[c + 1] += tot;
                } else {
                    down[r + 1][c] += tot / 2;
                    right[c + 1] += tot - tot / 2;
                }
            }
        } else {
            vl left(C + 1, 0);
            per(c, C - 1, 0) {
                ll in_top = down[r][c];
                ll in_right = left[c + 1];
                ll tot = in_top + in_right;
                if (tot == 0) continue;

                int choice = choices[r][c];
                if (c == 0) {
                    if (choice != 0) {
                        penalty += 1e12;
                        choice = 0;
                    }
                }

                if (in_top > 0 && in_right > 0) {
                    if (choice != 2) {
                        penalty += 1e9;
                    }
                }

                if (choice == 0) {
                    down[r + 1][c] += tot;
                } else if (choice == 1) {
                    left[c] += tot;
                } else {
                    down[r + 1][c] += tot / 2;
                    left[c] += tot - tot / 2;
                }
            }
        }
    }

    ll error = 0;
    rep(c, 0, C) {
        error += abs(down[R_grid + 1][c] - B[c]);
    }
    return error + penalty;
}

void solve() {
    if (!(cin >> C >> T_problem >> M)) return;
    A.resize(C);
    B.resize(C);
    rep(i, 0, C) cin >> A[i];
    rep(i, 0, C) cin >> B[i];

    R_grid = C + 20;

    vi row_bias(R_grid + 1, 0);
    vector<vi> choices(R_grid + 1, vi(C, 0));

    vi best_row_bias = row_bias;
    vector<vi> best_choices = choices;

    ll current_score = evaluate(row_bias, choices);
    ll best_score = current_score;

    auto start_time = chrono::high_resolution_clock::now();
    int iterations = 0;
    double T_sa = 10000.0;
    srand(1337);

    while (true) {
        iterations++;
        if (iterations % 1000 == 0) {
            auto current_time = chrono::high_resolution_clock::now();
            if (chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count() > 850) {
                break;
            }
        }

        int type = rand() % 2;
        if (type == 0) {
            int r = rand() % R_grid + 1;
            row_bias[r] ^= 1;
            ll new_score = evaluate(row_bias, choices);

            bool accept = false;
            if (new_score < current_score) accept = true;
            else {
                double p = exp((double)(current_score - new_score) / T_sa);
                if (((double)rand() / RAND_MAX) < p) accept = true;
            }

            if (accept) {
                current_score = new_score;
                if (new_score < best_score) {
                    best_score = new_score;
                    best_row_bias = row_bias;
                    best_choices = choices;
                }
            } else {
                row_bias[r] ^= 1;
            }
        } else {
            int r = rand() % R_grid + 1;
            int c = rand() % C;
            int old_choice = choices[r][c];
            choices[r][c] = rand() % 3;

            ll new_score = evaluate(row_bias, choices);

            bool accept = false;
            if (new_score < current_score) accept = true;
            else {
                double p = exp((double)(current_score - new_score) / T_sa);
                if (((double)rand() / RAND_MAX) < p) accept = true;
            }

            if (accept) {
                current_score = new_score;
                if (new_score < best_score) {
                    best_score = new_score;
                    best_row_bias = row_bias;
                    best_choices = choices;
                }
            } else {
                choices[r][c] = old_choice;
            }
        }
        T_sa *= 0.99995;
    }

    vector<vl> down(R_grid + 2, vl(C, 0));
    rep(c, 0, C) down[1][c] = A[c];

    vector<vector<string>> ans(R_grid + 1, vector<string>(C, "X"));

    rep(r, 1, R_grid + 1) {
        if (best_row_bias[r] == 0) {
            vl right(C + 1, 0);
            rep(c, 0, C) {
                ll in_top = down[r][c];
                ll in_left = right[c];
                ll tot = in_top + in_left;
                if (tot == 0) {
                    ans[r][c] = "X";
                    continue;
                }
                int choice = best_choices[r][c];
                if (c == C - 1) choice = 0;
                if (in_top > 0 && in_left > 0) choice = 2;

                if (choice == 0) {
                    ans[r][c] = "D";
                    down[r + 1][c] += tot;
                } else if (choice == 1) {
                    ans[r][c] = "R";
                    right[c + 1] += tot;
                } else {
                    ans[r][c] = "RD";
                    down[r + 1][c] += tot / 2;
                    right[c + 1] += tot - tot / 2;
                }
            }
        } else {
            vl left(C + 1, 0);
            per(c, C - 1, 0) {
                ll in_top = down[r][c];
                ll in_right = left[c + 1];
                ll tot = in_top + in_right;
                if (tot == 0) {
                    ans[r][c] = "X";
                    continue;
                }
                int choice = best_choices[r][c];
                if (c == 0) choice = 0;
                if (in_top > 0 && in_right > 0) choice = 2;

                if (choice == 0) {
                    ans[r][c] = "D";
                    down[r + 1][c] += tot;
                } else if (choice == 1) {
                    ans[r][c] = "L";
                    left[c] += tot;
                } else {
                    ans[r][c] = "LD";
                    down[r + 1][c] += tot / 2;
                    left[c] += tot - tot / 2;
                }
            }
        }
    }

    cout << R_grid << "\n";
    rep(r, 1, R_grid + 1) {
        rep(c, 0, C) {
            cout << ans[r][c] << (c == C - 1 ? "" : " ");
        }
        cout << "\n";
    }
}
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T = 1;
    while(T--) solve();
}