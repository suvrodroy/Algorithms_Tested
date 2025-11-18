struct SegTree {
    int n;
    vector<int> tree, lazy;
    // vector<bool> hasLazy; // uncomment for RANGE ASSIGN version

    SegTree(int _n) {
        n = _n;
        tree.assign(4 * n, 0);
        lazy.assign(4 * n, 0);
        // hasLazy.assign(4 * n, false); // for RANGE ASSIGN
    }

    int merge(int a, int b) {
        return a + b; // change if needed (min/max/etc)
    }

    void build(vector<int> &a, int node, int l, int r) {
        if (l == r) {
            tree[node] = a[l];
            return;
        }
        int mid = (l + r) / 2;
        build(a, node * 2, l, mid);
        build(a, node * 2 + 1, mid + 1, r);
        tree[node] = merge(tree[node * 2], tree[node * 2 + 1]);
    }

    void build(vector<int> &a) {
        build(a, 1, 0, n - 1);
    }

    // apply: used by both update() and push()
    void apply(int node, int l, int r, int val) {
        // --------- RANGE ADD VERSION ---------
        tree[node] += (r - l + 1) * val;
        lazy[node] += val;

        // --------- RANGE ASSIGN VERSION ---------
        // tree[node] = (r - l + 1) * val;
        // lazy[node] = val;
        // hasLazy[node] = true;
    }

    void push(int node, int l, int r) {
        if (l == r) return;

        // --------- RANGE ADD VERSION ---------
        if (lazy[node] == 0) return;
        int mid = (l + r) / 2;
        apply(node * 2,     l,     mid, lazy[node]);
        apply(node * 2 + 1, mid + 1, r, lazy[node]);
        lazy[node] = 0;

        // --------- RANGE ASSIGN VERSION ---------
        // if (!hasLazy[node]) return;
        // int mid = (l + r) / 2;
        // apply(node * 2,     l,     mid, lazy[node]);
        // apply(node * 2 + 1, mid + 1, r, lazy[node]);
        // hasLazy[node] = false;
    }

    void update(int node, int l, int r, int ql, int qr, int val) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) {
            apply(node, l, r, val);
            return;
        }
        push(node, l, r);
        int mid = (l + r) / 2;
        update(node * 2,     l,     mid, ql, qr, val);
        update(node * 2 + 1, mid + 1, r, ql, qr, val);
        tree[node] = merge(tree[node * 2], tree[node * 2 + 1]);
    }

    int query(int node, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0; // identity for sum
        if (ql <= l && r <= qr) return tree[node];
        push(node, l, r);
        int mid = (l + r) / 2;
        return merge(
            query(node * 2,     l,     mid, ql, qr),
            query(node * 2 + 1, mid + 1, r, ql, qr)
        );
    }

    // thin wrappers
    void update(int l, int r, int val) {
        update(1, 0, n - 1, l, r, val);
    }

    int query(int l, int r) {
        return query(1, 0, n - 1, l, r);
    }
};
