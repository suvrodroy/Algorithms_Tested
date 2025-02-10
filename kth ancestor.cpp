#include <bits/stdc++.h>
using namespace std;

class BinaryLifting {
public:
    int n, LOG;
    vector<vector<int>> up;
    vector<int> depth;

    BinaryLifting(int n, vector<int>& parent) {
        this->n = n;
        LOG = log2(n) + 1;
        up.assign(n, vector<int>(LOG, -1));
        depth.assign(n, 0);

        for (int v = 0; v < n; v++) {
            up[v][0] = parent[v];
            if (parent[v] != -1)
                depth[v] = depth[parent[v]] + 1;
        }

        for (int j = 1; j < LOG; j++) {
            for (int v = 0; v < n; v++) {
                if (up[v][j - 1] != -1)
                    up[v][j] = up[up[v][j - 1]][j - 1];
            }
        }
    }

    int getKthAncestor(int node, int k) {
        for (int j = 0; j < LOG; j++) {
            if (k & (1 << j)) {
                node = up[node][j];
                if (node == -1) break;
            }
        }
        return node;
    }
};

int main() {
    int n = 7;
    vector<int> parent = {-1, 0, 0, 1, 1, 2, 2};
    BinaryLifting bl(n, parent);

    cout << "3rd ancestor of node 6: " << bl.getKthAncestor(6, 3) << endl;
    cout << "2nd ancestor of node 4: " << bl.getKthAncestor(4, 2) << endl;

    return 0;
}
