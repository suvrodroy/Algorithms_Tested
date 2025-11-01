#include "bits/stdc++.h"
using namespace std;

struct AhoCorasick {
    static const int ALPHABET = 26;

    struct Node {
        array<int, ALPHABET> next; // trie edges
        array<int, ALPHABET> go;   // automaton transitions
        int link = -1;             // suffix link
        int out_link = -1;         // nearest terminal link
        vector<int> out;           // pattern indices ending here
        int p = -1;                // parent
        char pch;                  // char from parentc
        bool bad = false;

        Node(int parent = -1, char ch = '$') : p(parent), pch(ch) {
            next.fill(-1);
            go.fill(-1);
        }
    };

    vector<Node> t;

    AhoCorasick() { t.emplace_back(); }

    void add_string(const string &s, int id) {
        int v = 0;
        for (char ch : s) {
            int c = ch - 'a';
            if (t[v].next[c] == -1) {
                t[v].next[c] = t.size();
                t.emplace_back(v, ch);
            }
            v = t[v].next[c];
        }
        t[v].out.push_back(id);
    }

    void build() {
        queue<int> q;
        t[0].link = 0;
        for (int c = 0; c < ALPHABET; c++) {
            int u = t[0].next[c];
            if (u != -1) {
                t[u].link = 0;
                q.push(u);
            }
            t[0].go[c] = (u == -1 ? 0 : u); // initialize go transitions for root
        }

        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (int c = 0; c < ALPHABET; c++) {
                t[v].bad = (!t[v].out.empty()) || t[t[v].link].bad; // mark forbidden
                int u = t[v].next[c];
                if (u != -1) {
                    // compute suffix link of u
                    t[u].link = t[t[v].link].go[c];
                    // merge outputs
                    t[u].out_link = !t[t[u].link].out.empty() ? t[u].link : t[t[u].link].out_link;
                    q.push(u);
                }
                // always fill go transition
                t[v].go[c] = (u != -1 ? u : t[t[v].link].go[c]);
            }
        }
    }

    int go(int v, char ch) {
        return t[v].go[ch - 'a'];
    }

    // returns {pattern_id, ending_position}
    vector<pair<int,int>> find_all(const string &text) {
        vector<pair<int,int>> res;
        int v = 0;
        for (int i = 0; i < (int)text.size(); i++) {
            v = go(v, text[i]);
            for (int u = v; u != -1; u = t[u].out_link) {
                for (int id : t[u].out) {
                    res.emplace_back(id, i);
                }
            }
        }
        return res;
    }

    string lexicographically_smallest(AhoCorasick &ac, int L) {
        string res;
        int v = 0; // start at root
        for (int i = 0; i < L; i++) {
            for (int c = 0; c < ALPHABET; c++) {
                int u = ac.go(v, 'a' + c);
                if (!ac.t[u].bad) { // avoid forbidden states
                    res.push_back('a' + c);
                    v = u;
                    break;
                }
            }
        }
        return res;
    }

};
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t; cin >> t;
    while (t--)
    {
        string s,x;
        cin >> s;
        AhoCorasick ac;
        int n,m;
        cin >> n;
        for (int i = 0; i < n; i++)
        {
            cin >> x;
            ac.add_string(x,i);
        }
        ac.build();
        auto a = ac.find_all(s);
        vector<char> ans(n,'n');
        for (auto i : a)
            ans[i.first] = 'y';
        for (auto i : ans) cout << i << endl;
    }

}
