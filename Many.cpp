#include "bits/stdc++.h"
using namespace std;

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

// nCr Calculation
vint fact(N);
void factorialCalc()
{
    fact[0] = fact[1] = 1;
    for (int i = 2;i < N; i++)
        fact[i] = (fact[i-1] * i)%Mod;
}
int binpow(long long a, long long b) {
    long long res = 1;
    a %= Mod;
    while (b > 0) {
        if (b & 1)
            res = (res * a) % Mod;
        a = (a * a) % Mod;
        b >>= 1;
    }
    return res;
}
int nCr(int nn, int r)
{
    if (r > nn || r < 0) return 0;
    int nom = fact[nn];
    int denom = (fact[r] * fact[nn - r]) % Mod;
    denom = binpow(denom, Mod - 2);
    return (nom * denom) % Mod;
}
// End Of nCr Calculation
// Template for Ordered Set
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace __gnu_pbds;
template <typename T>
using ordered_set = tree<
    T,                      // Key type
    null_type,              // Mapped policy (null for set)
    less<T>,                // Key comparison
    rb_tree_tag,            // Data structure (Red-Black Tree)
    tree_order_statistics_node_update>; // Order-statistics

/*
Useful Methods:
- s.insert(x): inserts x
- s.erase(x): erases x
- s.find_by_order(k): returns iterator to k-th element (0-indexed)
- s.order_of_key(x): returns number of elements strictly less than x
*/

//END of template

// Ordered Multiset
using namespace __gnu_pbds;

template <typename T>
class OrderedMultiset {
private:
    using pii = pair<T, int>;
    tree<pii, null_type, less<pii>, rb_tree_tag, tree_order_statistics_node_update> os;
    int uid = 0; // Unique ID for handling duplicates
public:
    // Insert a value
    void insert(T x) {
        os.insert({x, uid++});
    }

    // Erase one occurrence of a value
    bool erase(T x) {
        auto it = os.lower_bound({x, 0});
        if (it != os.end() && it->first == x) {
            os.erase(it);
            return true;
        }
        return false;
    }
    // Count how many times x appears
    int count(T x) {
        return order_of_key_upper(x) - order_of_key(x);
    }
    // Number of elements strictly less than x
    int order_of_key(T x) {
        return os.order_of_key({x, 0});
    }
    // Number of elements ≤ x
    int order_of_key_upper(T x) {
        return os.order_of_key({x, INT_MAX});
    }
    // K-th smallest (0-indexed). Returns -1 if k is invalid
    T find_by_order(int k) {
        if (k < 0 || k >= (int)os.size()) return -1; // Or throw exception
        return os.find_by_order(k)->first;
    }
    // Size of multiset
    int size() const {
        return os.size();
    }
    // Check if x exists
    bool contains(T x) {
        return count(x) > 0;
    }
    // Clear all elements
    void clear() {
        os.clear();
        uid = 0;
    }
    // Print contents for debugging
    void print() {
        for (auto [val, _] : os) cout << val << " ";
        cout << "\n";
    }
};


void solve(){
    OrderedMultiset<int> ms;

    ms.insert(5);
    ms.insert(5);
    ms.insert(10);
    ms.insert(7);

    cout << "Count of 5: " << ms.count(5) << "\n";                  // 2
    cout << "Order of key 7: " << ms.order_of_key(7) << "\n";       // 2
    cout << "Order of key ≤ 7: " << ms.order_of_key_upper(7) << "\n"; // 3
    cout << "3rd smallest: " << ms.find_by_order(2) << "\n";        // 7

    ms.erase(5); // Remove one occurrence
    cout << "Count of 5 after erase: " << ms.count(5) << "\n";       // 1

    cout << "Contains 10: " << (ms.contains(10) ? "Yes" : "No") << "\n";

    ms.print(); // For debugging
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