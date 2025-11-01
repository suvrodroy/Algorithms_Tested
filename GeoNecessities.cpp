#include "bits/stdc++.h"
using namespace std;

// #pragma GCC optimize("O3,unroll-loops")
// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#define int long long
#define dub long double
typedef vector<int> vint;
typedef vector<string> vstr;
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

// Given three points p1, p2, p3:
// Vector u = p1 - p2  →  (ux, uy) = (p1.x - p2.x, p1.y - p2.y)
// Vector v = p3 - p2  →  (vx, vy) = (p3.x - p2.x, p3.y - p2.y)
//
// Dot product formula:
// dot(u, v) = ux * vx + uy * vy
//            = (p1.x - p2.x)*(p3.x - p2.x) + (p1.y - p2.y)*(p3.y - p2.y)
//
// Cross product (2D scalar form):
// cross(u, v) = ux * vy - uy * vx
//              = (p1.x - p2.x)*(p3.y - p2.y) - (p1.y - p2.y)*(p3.x - p2.x)
//
// Angle between u and v (in radians):
// θ = atan2(|cross(u, v)|, dot(u, v))

struct pt
{
    double x,y;
};

int orientation(pt a, pt b, pt c) {
    double v = a.x*(b.y-c.y)+b.x*(c.y-a.y)+c.x*(a.y-b.y);
    if (v < 0) return -1; // clockwise
    if (v > 0) return +1; // counter-clockwise
    return 0;
}
//Inner Angle of convex hull made by points p1,p2,p3 in anti-clockwise order
double angle_at(const pt& p1, const pt& p2, const pt& p3) {
    // vectors p2->p1 and p2->p3
    double ux = p1.x - p2.x, uy = p1.y - p2.y;
    double vx = p3.x - p2.x, vy = p3.y - p2.y;

    // handle degenerate cases (duplicate points)
    if ((ux == 0 && uy == 0) || (vx == 0 && vy == 0)) return 0.0;

    double cross = ux*vy - uy*vx;      // |u||v|sinθ (signed)
    double dot   = ux*vx + uy*vy;      // |u||v|cosθ

    // cout << "Theta = " << atan2(fabs(cross), dot) << endl;

    return atan2(fabs(cross), dot);  // θ in [0, π]
}

// Finds angle (in radians) between line p1p2 and line p3p4
double angle_between_lines(const pt& p1, const pt& p2, const pt& p3, const pt& p4) {
    // Direction vectors of both lines
    double ux = p2.x - p1.x;
    double uy = p2.y - p1.y;
    double vx = p4.x - p3.x;
    double vy = p4.y - p3.y;

    // Compute dot and cross products
    double dot = ux * vx + uy * vy;        // dot(u, v)
    double cross = ux * vy - uy * vx;      // cross(u, v)

    // If one of the lines is degenerate (both points same)
    if ((ux == 0 && uy == 0) || (vx == 0 && vy == 0))
        return 0.0;

    // Angle in radians between 0 and π
    double theta = atan2(fabs(cross), dot);
    return theta;
}

void solve(){

}
signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int test_n = 1;
    cin>>test_n;
    for(int t_c = 1;t_c <= test_n;t_c ++){
        solve();
    }
    return 0;
}