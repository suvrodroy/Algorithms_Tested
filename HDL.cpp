struct SegmentTree {
    int n;
    vector<int> tree;

    // Change this function to switch between XOR / MAX / MIN / SUM etc.
    int combine(int a, int b) {
        return a ^ b; // Change to a + b for SUM, min(a,b) for MIN, max(a,b) for MAX
    }

    SegmentTree(int sz) {
        n = sz;
        tree.assign(4 * n, 0);
    }

    void build(vector<int> &arr, int v, int tl, int tr) {
        if (tl == tr) {
            tree[v] = arr[tl];
        } else {
            int tm = (tl + tr) / 2;
            build(arr, 2*v, tl, tm);
            build(arr, 2*v+1, tm+1, tr);
            tree[v] = combine(tree[2*v], tree[2*v+1]);
        }
    }

    void update(int v, int tl, int tr, int pos, int val) {
        if (tl == tr) {
            tree[v] = val;
        } else {
            int tm = (tl + tr) / 2;
            if (pos <= tm)
                update(2*v, tl, tm, pos, val);
            else
                update(2*v+1, tm+1, tr, pos, val);
            tree[v] = combine(tree[2*v], tree[2*v+1]);
        }
    }

    int query(int v, int tl, int tr, int l, int r) {
        if (l > r) return 0; // Change to INF or -INF for MIN/MAX accordingly
        if (l == tl && r == tr) return tree[v];
        int tm = (tl + tr) / 2;
        return combine(
            query(2*v, tl, tm, l, min(r, tm)),
            query(2*v+1, tm+1, tr, max(l, tm+1), r)
        );
    }
};
