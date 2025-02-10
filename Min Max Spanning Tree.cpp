/// Problem L OJ Civil and Evil Engineer
/// Solution: average of total edge wieght of Max Spanning Tree and Min Spanning Tree

#include "bits/stdc++.h"
using namespace std;

#define int long long
#define rep(i,j,k) for(int i=j;i<k;i++)
#define all(a) a.begin(),a.end()
#define endl '\n'
#define INF 2147483647
//#define INF 9223372036854775807
#define null '\0'
const int N = 1e5 + 10, Mod = 1e9 + 7;
int n,m;
vector<int> parent, Rank;

void make_set(int v) {
    parent[v] = v;
    Rank[v] = 0;
}

int find_set(int v) {
    if (v == parent[v])
        return v;
    return parent[v] = find_set(parent[v]);
}

void union_sets(int a, int b) {
    a = find_set(a);
    b = find_set(b);
    if (a != b) {
        if (Rank[a] < Rank[b])
            swap(a, b);
        parent[b] = a;
        if (Rank[a] == Rank[b])
            Rank[a]++;
    }
}

struct Edge {
    int u, v, weight;
    bool operator<(Edge const& other) {
        return weight < other.weight;
    }
};
void solve(){
    cin >> n;
    vector<Edge> edges;
    int a,b,c;
    while(cin >> a >> b >> c) {
        if(a == 0 and a == b and b == c) break;
        edges.push_back({a,b,c});
    }
    int cost = 0;
    vector<Edge> result;
    parent.resize(n+1);
    Rank.resize(n+1);
    for (int i = 0; i <= n; i++)
        make_set(i);
    sort(edges.begin(), edges.end()); /// Increasing order for min spanning tree
    for (Edge e : edges) {
        if (find_set(e.u) != find_set(e.v)) {
            cost += e.weight;
            ///result.push_back(e);
            union_sets(e.u, e.v);
        }
    }
    a = cost;
    cost = 0;
    parent.resize(n+1);
    Rank.resize(n+1);
    for (int i = 0; i <= n; i++)
        make_set(i);
    reverse(edges.begin(), edges.end()); /// Descenging order for max spanning tree
    for (Edge e : edges) {
        if (find_set(e.u) != find_set(e.v)) {
            cost += e.weight;
            union_sets(e.u, e.v);
        }
    }
    b = cost;
    c = a+b;
    if(c%2) cout << c <<"/2\n";
    else cout << c/2<<endl;
}
signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int test_n = 1;
    cin>>test_n;
    for(int t_c = 1;t_c <= test_n;t_c ++){
        cout << "Case " << t_c << ": ";
        solve();
    }
    return 0;
}