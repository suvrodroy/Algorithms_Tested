#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
public:
    vector<int> tree, lazy;
    int n;

    SegmentTree(int size) {
        n = size;
        tree.resize(4 * n);
        lazy.resize(4 * n);
    }

    void buildMin(vector<int>& arr, int node, int start, int end) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            int mid = (start + end) / 2;
            buildMin(arr, 2 * node, start, mid);
            buildMin(arr, 2 * node + 1, mid + 1, end);
            tree[node] = min(tree[2 * node], tree[2 * node + 1]);
        }
    }

    void buildMax(vector<int>& arr, int node, int start, int end) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            int mid = (start + end) / 2;
            buildMax(arr, 2 * node, start, mid);
            buildMax(arr, 2 * node + 1, mid + 1, end);
            tree[node] = max(tree[2 * node], tree[2 * node + 1]);
        }
    }

    void updateLazy(int node, int start, int end, int l, int r, int value) {
        if (lazy[node] != 0) {
            tree[node] += lazy[node];
            if (start != end) {
                lazy[2 * node] += lazy[node];
                lazy[2 * node + 1] += lazy[node];
            }
            lazy[node] = 0;
        }

        if (start > end || start > r || end < l) return;

        if (start >= l && end <= r) {
            tree[node] += value;
            if (start != end) {
                lazy[2 * node] += value;
                lazy[2 * node + 1] += value;
            }
            return;
        }

        int mid = (start + end) / 2;
        updateLazy(2 * node, start, mid, l, r, value);
        updateLazy(2 * node + 1, mid + 1, end, l, r, value);
        tree[node] = min(tree[2 * node], tree[2 * node + 1]);
    }

    int query(int node, int start, int end, int l, int r) {
        if (lazy[node] != 0) {
            tree[node] += lazy[node];
            if (start != end) {
                lazy[2 * node] += lazy[node];
                lazy[2 * node + 1] += lazy[node];
            }
            lazy[node] = 0;
        }

        if (start > end || start > r || end < l) return INT_MAX;
        if (start >= l && end <= r) return tree[node];

        int mid = (start + end) / 2;
        return min(query(2 * node, start, mid, l, r), query(2 * node + 1, mid + 1, end, l, r));
    }
};

int main() {
    vector<int> arr = {1, 3, -2, 8, -7, 6};
    int n = arr.size();

    SegmentTree minTree(n);
    minTree.buildMin(arr, 1, 0, n - 1);

    SegmentTree maxTree(n);
    maxTree.buildMax(arr, 1, 0, n - 1);

    SegmentTree lazyTree(n);
    lazyTree.buildMin(arr, 1, 0, n - 1);

    cout << "Min query (0,4): " << minTree.query(1, 0, n - 1, 0, 4) << endl;
    cout << "Max query (1,3): " << maxTree.query(1, 0, n - 1, 1, 3) << endl;

    lazyTree.updateLazy(1, 0, n - 1, 1, 4, 3);
    cout << "Lazy updated min query (0,4): " << lazyTree.query(1, 0, n - 1, 0, 4) << endl;

    return 0;
}