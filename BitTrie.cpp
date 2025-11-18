#include "bits/stdc++.h"
using namespace std;

struct BitTrie {
    struct Node {
        int ch[2];
        int cnt;
        Node() { ch[0] = ch[1] = -1; cnt = 0; }
    };

    vector<Node> t;
    int B;              // number of bits to use

    BitTrie(int _B) {   // YOU choose max bit here
        B = _B;
        t.reserve(1 << 20);
        t.push_back(Node()); // root
    }

    // Clear / reuse
    void clear() {
        t.clear();
        t.push_back(Node());
    }

    // Insert a number
    void insert(long long x) {
        int v = 0;
        t[v].cnt++;
        for (int b = B; b >= 0; --b) {
            int bit = (x >> b) & 1LL;
            int &nx = t[v].ch[bit];
            if (nx == -1) {
                nx = t.size();
                t.push_back(Node());
            }
            v = nx;
            t[v].cnt++;
        }
    }

    // Contains check
    bool contains(long long x) {
        int v = 0;
        if (t[v].cnt == 0) return false;
        for (int b = B; b >= 0; --b) {
            int bit = (x >> b) & 1LL;
            int nx = t[v].ch[bit];
            if (nx == -1) return false;
            v = nx;
            if (t[v].cnt == 0) return false;
        }
        return true;
    }

    // Erase one occurrence
    bool erase(long long x) {
        if (!contains(x)) return false;
        int v = 0;
        t[v].cnt--;
        for (int b = B; b >= 0; --b) {
            int bit = (x >> b) & 1LL;
            int nx = t[v].ch[bit];
            v = nx;
            t[v].cnt--;
        }
        return true;
    }

    // Max XOR query (returns x ^ best_value)
    long long max_xor(long long x) {
        int v = 0;
        if (t[v].cnt == 0) return 0;
        long long ans = 0;
        for (int b = B; b >= 0; --b) {
            int bit = (x >> b) & 1LL;
            int want = bit ^ 1;
            int nx = t[v].ch[want];
            if (nx != -1 && t[nx].cnt > 0) {
                ans |= (1LL << b);
                v = nx;
            } else {
                nx = t[v].ch[bit];
                if (nx == -1) break;
                v = nx;
            }
        }
        return ans;
    }

    // Min XOR query
    long long min_xor(long long x) {
        int v = 0;
        if (t[v].cnt == 0) return 0;
        long long ans = 0;
        for (int b = B; b >= 0; --b) {
            int bit = (x >> b) & 1LL;
            int want = bit;
            int nx = t[v].ch[want];
            if (nx != -1 && t[nx].cnt > 0) {
                v = nx;
            } else {
                nx = t[v].ch[want ^ 1];
                if (nx == -1) break;
                ans |= (1LL << b);
                v = nx;
            }
        }
        return ans;
    }

    // Returns actual trie value that maximizes XOR
    long long best_xor_value(long long x) {
        return x ^ max_xor(x);
    }
};
int main() {
    BitTrie bt(60); // work with 60 bits

    bt.insert(5);
    bt.insert(10);
    bt.insert(7);

    long long x = 6;
    cout << bt.max_xor(x) << "\n";         // XOR value
    cout << bt.best_xor_value(x) << "\n";  // actual number
}
