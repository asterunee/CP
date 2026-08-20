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

void solve(){
    int n, t, m;
    if(!(cin >> n >> t >> m)) return;
    
    vl a(n), b(n);
    rep(i, 0, n) cin >> a[i];
    rep(i, 0, n) cin >> b[i];

    vector<pair<ll, int>> p(n), q(n);
    rep(i, 0, n){
        p[i] = {a[i], i};
        q[i] = {b[i], i};
    }
    
    sort(all(p));
    sort(all(q));

    vi p_tgt(n);
    rep(i, 0, n){
        p_tgt[p[i].second] = q[i].second;
    }

    int h = n + 1; 
    vector<vector<string>> g(h, vector<string>(n, "X"));

    rep(i, 0, n){
        int v = p_tgt[i];
        if(v == i){
            g[0][i] = to_string(h) + "D";
        } else {
            int y = i + 1; 
            if(y == 1) g[0][i] = "D";
            else g[0][i] = to_string(y) + "D";
            
            if(v > i){
                if(v - i == 1) g[y][i] = "R";
                else g[y][i] = to_string(v - i) + "R";
            } else {
                if(i - v == 1) g[y][i] = "L";
                else g[y][i] = to_string(i - v) + "L";
            }
            
            if(h - y == 1) g[y][v] = "D";
            else g[y][v] = to_string(h - y) + "D";
        }
    }

    cout << h << "\n";
    rep(i, 0, h){
        rep(j, 0, n){
            cout << g[i][j] << (j == n - 1 ? "" : " ");
        }
        cout << "\n";
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T=1;
    while(T--)solve();
}