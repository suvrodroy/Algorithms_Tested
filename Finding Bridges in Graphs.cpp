/// L OJ Critical Links
/// Solution: Just find bridges in the Graph
/// Bridge: An edge if removed makes the graph disconnected

#include "bits/stdc++.h"
using namespace std;

#define int long long
#define dub long double
typedef vector<int> vint;
typedef pair<int,int> pii;
typedef vector<pii> vii;
typedef unordered_map<int,int> umap;
#define rep(i,j,k) for(int i=j;i<k;i++)
#define all(a) a.begin(),a.end()
#define endl '\n'
#define INF 2147483647
//#define INF 9223372036854775807
#define null '\0'
const int N = 1e5 + 10, Mod = 1e9 + 7;

int n,m;
vector<vector<int>> g;
vector<pii> edges;
vector<bool> visited;
vector<int> tin, low;
int timer;
void IS_BRIDGE(int u,int v) {
    if(u > v) swap(u,v);
    edges.push_back({u,v});
}
void dfs(int v, int p = -1) {
    visited[v] = true;
    tin[v] = low[v] = timer++;
    for (int to : g[v]) {
        if (to == p) continue;
        if (visited[to]) {
            low[v] = min(low[v], tin[to]);
        } else {
            dfs(to, v);
            low[v] = min(low[v], low[to]);
            if (low[to] > tin[v])
                IS_BRIDGE(v, to);
        }
    }
}
void find_bridges() {
    timer = 0;
    visited.assign(n, false);
    tin.assign(n, -1);
    low.assign(n, -1);
    for (int i = 0; i < n; ++i) {
        if (!visited[i])
            dfs(i);
    }
}
int convert(string s) {
    int num = 0;
    for(int i = 1;i < s.size() - 1; i++) {
        num = num*10 + s[i] - '0';
    }
    return num;
}
void solve(){
    edges.clear();
    g.clear();
    cin >> n;
    g.resize(n);
    int a,b,c;
    vint parent(n+2 , -1);
    vii e;
    map<pii, bool> mp;
    rep(i,0,n) {
        cin >> a;
        string s; cin>> s;
        m = convert(s);
        while(m--) {
            cin >> b;
            g[a].push_back(b);
            g[b].push_back(a);
        }
    }
    find_bridges();
    sort(all(edges));
    cout << edges.size() << " critical links\n";
    for(auto i : edges) {
        cout << i.first << " - " << i.second << endl;
    }

}
signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int test_n = 1;
    cin>>test_n;
    for(int t_c = 1;t_c <= test_n;t_c ++){
        cout << "Case " << t_c << ":\n";
        solve();
    }
    return 0;
}