// Dynamic Convex Hull of POINTS (insert-only) + membership query
// C++17, integer coords. Amortized O(log n) per insert, O(log n) per query.
// Keep two chains (lower, upper), each strictly increasing in x.
// Query: point is inside/on iff not below lower AND not above upper.

#include <bits/stdc++.h>
using namespace std;

struct DynHull {
    struct P {
        long long x, y;
        bool operator<(const P& o) const { return x < o.x || (x == o.x && y < o.y); }
        bool operator==(const P& o) const { return x == o.x && y == o.y; }
    };

    static inline long long cross(const P& a, const P& b, const P& c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }
    static inline bool onSeg(const P& a, const P& b, const P& p) {
        if (cross(a,b,p)) return false;
        return min(a.x,b.x) <= p.x && p.x <= max(a.x,b.x)
            && min(a.y,b.y) <= p.y && p.y <= max(a.y,b.y);
    }

    set<P> lower, upper;  // x-strict chains
    set<P> all;           // unique points

    // insert p into S with x-uniqueness: keep_extreme(cur,cand) tells if 'cur' wins
    template<class KeepExtreme>
    static typename set<P>::iterator insert_xstrict(set<P>& S, P p, KeepExtreme keep_extreme) {
        auto it = S.lower_bound(P{p.x, LLONG_MIN});
        if (it != S.end() && it->x == p.x) {               // same x exists
            if (keep_extreme(*it, p)) return S.end();      // no change
            it = S.erase(it), it = S.insert(p).first;      // replace
        } else it = S.insert(p).first;
        return it;
    }

    template<class BadTurn>
    static void prune(set<P>& S, typename set<P>::iterator it, BadTurn bad) {
        // left side
        while (it != S.begin()) {
            auto m = prev(it);
            if (m == S.begin()) break;
            auto l = prev(m);
            if (bad(*l, *m, *it)) S.erase(m);
            else break;
        }
        // right side
        while (true) {
            auto m = next(it);
            if (m == S.end()) break;
            auto r = next(m);
            if (r == S.end()) break;
            if (bad(*it, *m, *r)) S.erase(m);
            else break;
        }
    }

    bool add(long long x, long long y) {
        P p{x,y};
        if (!all.insert(p).second) return false;

        // lower: keep minimal y at each x; bad turn = non-CCW
        if (auto it = insert_xstrict(lower, p, [](const P& cur, const P& cand){ return cand.y >= cur.y; });
            it != lower.end()) prune(lower, it, [](const P& a, const P& b, const P& c){ return cross(a,b,c) <= 0; });

        // upper: keep maximal y at each x; bad turn = non-CW
        if (auto it = insert_xstrict(upper, p, [](const P& cur, const P& cand){ return cand.y <= cur.y; });
            it != upper.end()) prune(upper, it, [](const P& a, const P& b, const P& c){ return cross(a,b,c) >= 0; });

        return true;
    }

    // return -1 if outside on bad side, 0 on boundary, +1 inside-side relative to chain
    static int side(const set<P>& S, const P& p, bool isLower) {
        if (S.empty()) return -1;
        if (S.size() == 1) return (p == *S.begin()) ? 0 : -1;

        const P &L0 = *S.begin(), &R0 = *S.rbegin();
        if (p.x < L0.x || p.x > R0.x) return -1;

        auto it = S.lower_bound(P{p.x, LLONG_MIN});
        if (it == S.end()) {
            auto b = *prev(S.end());
            auto a = *prev(prev(S.end()));
            if (onSeg(a,b,p)) return 0;
            long long cr = cross(a,b,p);
            return isLower ? (cr > 0 ? +1 : -1) : (cr < 0 ? +1 : -1);
        }
        if (it->x == p.x) {
            if (p.y == it->y) return 0;
            return isLower ? (p.y > it->y ? +1 : (p.y < it->y ? -1 : 0))
                           : (p.y < it->y ? +1 : (p.y > it->y ? -1 : 0));
        }
        auto R = *it, L = *prev(it);
        if (onSeg(L,R,p)) return 0;
        long long cr = cross(L,R,p);
        return isLower ? (cr > 0 ? +1 : -1) : (cr < 0 ? +1 : -1);
    }

    bool contains(long long x, long long y) const {
        if (all.empty()) return false;
        if (all.size() == 1) return (*all.begin()).x == x && (*all.begin()).y == y;

        // segment degeneracy (two points total)
        if (all.size() == 2) {
            auto it = all.begin(); P a = *it, b = *next(it);
            return onSeg(a,b,P{x,y});
        }
        P p{x,y};
        int sL = side(lower, p, true);
        if (sL == -1) return false;
        int sU = side(upper, p, false);
        if (sU == -1) return false;
        return true;
    }
};

/*** Example (CF 70D style)
int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int q; cin >> q;
    DynHull H;
    while(q--){
        int t; long long x,y; cin >> t >> x >> y;
        if(t==1) H.add(x,y);
        else cout << (H.contains(x,y) ? "YES\n" : "NO\n");
    }
}
***/
