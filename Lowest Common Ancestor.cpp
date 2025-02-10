#include <bits/stdc++.h>
using namespace std;

class LCA {
public:
    int n, LOG;
    vector<vector<int>> up;
    vector<int> depth;

    LCA(int n, vector<vector<int>>& adj, int root = 0) {
        this->n = n;
        LOG = log2(n) + 1;
        up.assign(n, vector<int>(LOG, -1));
        depth.assign(n, 0);

        function<void(int, int)> dfs = [&](int v, int p) {
            up[v][0] = p;
            for (int j = 1; j < LOG; j++) {
                if (up[v][j - 1] != -1)
                    up[v][j] = up[up[v][j - 1]][j - 1];
            }
            for (int u : adj[v]) {
                if (u == p) continue;
                depth[u] = depth[v] + 1;
                dfs(u, v);
            }
        };

        dfs(root, -1);
    }

    int getLCA(int u, int v) {
        if (depth[u] < depth[v]) swap(u, v);
        int diff = depth[u] - depth[v];
        for (int j = 0; j < LOG; j++) {
            if (diff & (1 << j)) {
                u = up[u][j];
            }
        }
        if (u == v) return u;
        for (int j = LOG - 1; j >= 0; j--) {
            if (up[u][j] != up[v][j]) {
                u = up[u][j];
                v = up[v][j];
            }
        }
        return up[u][0];
    }
};

int main() {
    int n = 7;
    vector<vector<int>> adj(n);
    vector<pair<int, int>> edges = {{0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {2, 6}};

//                                0
//                               / \
//                              1   2
//                             / \  | \
//                            3   4 5  6
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    LCA lca(n, adj);

    cout << "LCA of 3 and 4: " << lca.getLCA(3, 4) << endl;
    cout << "LCA of 5 and 6: " << lca.getLCA(5, 6) << endl;
    cout << "LCA of 3 and 6: " << lca.getLCA(3, 6) << endl;

    return 0;
}
