#include <bits/stdc++.h>
using namespace std;

/*
  ============================
  Suffix Array (SA-IS) Template
  ============================

  WHAT YOU GET
  ------------
  - Suffix array via SA-IS in O(n)
  - LCP array via Kasai in O(n)
  - RMQ over LCP via Sparse Table in O(n log n)
  - get_lcp(i, j): LCP of suffixes starting at i and j in O(1)
  - Pattern search: lower/upper bound and occurrence range
  - Contest-ready I/O and safe alphabet handling

  TWEAKABLE KNOBS
  ---------------
  - ALPHABET: max integer symbol + 1 for SA-IS. For 8-bit input use 257.
  - WANT_RMQ: if you don't need range LCP queries, you can disable the sparse table to save memory.

  NAMING CONVENTIONS
  ------------------
  - text: integer-mapped input string *with* sentinel 0 at the end.
  - sa: suffix array, positions 0..n-1 of suffixes in sorted order.
  - rankAt[pos]: position of suffix 'pos' in 'sa' (inverse SA).
  - lcp[i]: LCP(sa[i], sa[i+1]) for i in [0..n-2].
*/

namespace SAIS {

  // ======= Helper: Induced Sort =======
  // Places LMS suffixes first, then induces L-type and S-type suffixes.
  // text        : integer array with sentinel 0 at the end
  // alphabet    : one more than the maximum symbol in text
  // sa          : output suffix array (filled here)
  // isLType     : true if L-type, false if S-type
  // lmsPositions: indices i where S[i] is LMS ( !isLType[i] && isLType[i-1] )
  static void induced_sort(const vector<int>& text, int alphabet,
                           vector<int>& sa,
                           const vector<bool>& isLType,
                           const vector<int>& lmsPositions) {
    const int n = (int)text.size();
    vector<int> bucketStart(alphabet, 0), bucketEnd(alphabet, 0);

    // Count occurrences to get bucket boundaries
    for (int c : text) ++bucketEnd[c];
    partial_sum(bucketEnd.begin(), bucketEnd.end(), bucketEnd.begin()); // exclusive end
    bucketStart[0] = 0;
    for (int c = 1; c < alphabet; ++c) bucketStart[c] = bucketEnd[c-1];

    auto resetStarts = [&]() {
      bucketStart[0] = 0;
      for (int c = 1; c < alphabet; ++c) bucketStart[c] = bucketEnd[c-1];
    };
    auto resetEnds = [&]() {
      partial_sum(bucketEnd.begin(), bucketEnd.end(), bucketEnd.begin());
    };

    fill(sa.begin(), sa.end(), -1);

    // Step 1: place LMS at the ends of buckets (stable from right to left)
    vector<int> tail = bucketEnd;
    for (int i = (int)lmsPositions.size() - 1; i >= 0; --i) {
      int p = lmsPositions[i];
      sa[--tail[text[p]]] = p;
    }

    // Step 2: induce L-type from left to right to heads of buckets
    vector<int> head = bucketStart;
    for (int idx = 0; idx < n; ++idx) {
      int p = sa[idx];
      if (p > 0 && isLType[p - 1]) {
        sa[head[text[p - 1]]++] = p - 1;
      }
    }

    // Step 3: induce S-type from right to left to tails of buckets
    // Need fresh tail boundaries based on counts
    vector<int> count(alphabet, 0);
    for (int c : text) ++count[c];
    for (int c = 0, acc = 0; c < alphabet; ++c) {
      acc += count[c];
      bucketEnd[c] = acc;
    }
    tail = bucketEnd;

    for (int idx = n - 1; idx >= 0; --idx) {
      int p = sa[idx];
      if (p > 0 && !isLType[p - 1]) {
        sa[--tail[text[p - 1]]] = p - 1;
      }
    }
  }

  // ======= Core SA-IS =======
  // Builds suffix array for integer array 'text' ending with 0 (sentinel),
  // with all other symbols in [1..alphabet-1].
  static vector<int> sa_is(const vector<int>& text, int alphabet) {
    const int n = (int)text.size();
    vector<int> sa(n, -1);                // output SA
    vector<bool> isLType(n, false);       // L/S types; true = L-type
    vector<int> lmsPositions;             // LMS indices

    // Classify types: last is S-type by convention (isLType[n-1] = false)
    for (int i = n - 2; i >= 0; --i) {
      // L-type if text[i] > text[i+1] or equal and next is L-type
      isLType[i] = (text[i] > text[i + 1]) || (text[i] == text[i + 1] && isLType[i + 1]);
      if (isLType[i] && !isLType[i + 1]) lmsPositions.push_back(i + 1);
    }
    reverse(lmsPositions.begin(), lmsPositions.end());

    // First induced sort using LMS seeds
    induced_sort(text, alphabet, sa, isLType, lmsPositions);

    // Extract LMS order as it appears in SA
    vector<int> sortedLMS;
    sortedLMS.reserve(lmsPositions.size());
    for (int p : sa) {
      if (p >= 1 && !isLType[p] && isLType[p - 1]) sortedLMS.push_back(p);
    }

    // Name LMS substrings
    // Assign lexicographic names to contiguous LMS substrings; equal substrings share a name.
    int m = (int)lmsPositions.size();
    vector<int> nameOfPos(n, -1); // nameOfPos[pos] = name id if pos is LMS
    int currentName = 0;
    nameOfPos[sortedLMS[0]] = 0;

    auto is_lms = [&](int i) -> bool {
      return i > 0 && !isLType[i] && isLType[i - 1];
    };
    auto equal_lms_substring = [&](int a, int b) -> bool {
      if (text[a] != text[b]) return false;
      int i = a + 1, j = b + 1;
      while (true) {
        bool aLMS = is_lms(i);
        bool bLMS = is_lms(j);
        if (text[i] != text[j]) return false;
        if (aLMS || bLMS) return aLMS && bLMS; // end matching exactly at both LMS
        ++i; ++j;
      }
    };

    for (int k = 1; k < (int)sortedLMS.size(); ++k) {
      int prev = sortedLMS[k - 1];
      int cur  = sortedLMS[k];
      if (!equal_lms_substring(prev, cur)) ++currentName;
      nameOfPos[cur] = currentName;
    }

    // Build reduced string of LMS names in the *original LMS order*
    vector<int> reduced(m);
    for (int i = 0; i < m; ++i) reduced[i] = nameOfPos[lmsPositions[i]];

    vector<int> saLMS; // SA of reduced problem -> order of LMS indices
    if (currentName + 1 == m) {
      // All names are unique; reduced SA is trivial: order by name
      saLMS.resize(m);
      iota(saLMS.begin(), saLMS.end(), 0);
      sort(saLMS.begin(), saLMS.end(), [&](int a, int b){ return reduced[a] < reduced[b]; });
    } else {
      // Recurse on reduced problem
      // Alphabet for reduced is currentName+1, add sentinel at end
      vector<int> reducedText(m + 1);
      for (int i = 0; i < m; ++i) reducedText[i] = reduced[i] + 1;
      reducedText[m] = 0;
      auto saReduced = sa_is(reducedText, currentName + 2); // +1 for names, +1 for sentinel
      saReduced.erase(saReduced.begin()); // drop sentinel

      // Map back: saLMS holds LMS indices in SA order
      saLMS.resize(m);
      for (int i = 0; i < m; ++i) saLMS[i] = lmsPositions[saReduced[i]];
    }

    // Final induced sort using correctly ordered LMS positions
    induced_sort(text, alphabet, sa, isLType, saLMS);
    return sa;
  }

} // namespace SAIS

// ======= Public SuffixArray struct =======
// Build once, then query rank/LCP/patterns quickly.
struct SuffixArray {
  int n = 0;                       // length of original string
  string s;                        // original string (without sentinel)
  vector<int> sa;                  // sa[i] = starting index of i-th smallest suffix
  vector<int> rankAt;              // rankAt[pos] = position of suffix 'pos' in sa
  vector<int> lcp;                 // lcp[i] = LCP(sa[i], sa[i+1]) for i in [0..n-2]

  // Sparse Table for RMQ over lcp (optional but typical for O(1) LCP queries)
  vector<vector<int>> st;          // st[k][i] = min over lcp[i..i+2^k-1]
  vector<int> lg2;                 // lg2[i] = floor(log2(i))
  bool WANT_RMQ = true;            // toggle if needed

  SuffixArray() {}
  explicit SuffixArray(const string& str, bool build_rmq = true) {
    build(str, build_rmq);
  }

  // --- Build all data structures ---
  // 1) SA with SA-IS
  // 2) rankAt with inverse SA
  // 3) lcp via Kasai
  // 4) optional RMQ over lcp
  void build(const string& str, bool build_rmq = true) {
    s = str; n = (int)s.size();

    // Integer map: [1..256], sentinel 0 at the end
    const int ALPHABET = 257; // 256 chars + sentinel
    vector<int> text(n + 1, 0);
    for (int i = 0; i < n; ++i) text[i] = (unsigned char)s[i] + 1;
    text[n] = 0;

    sa = SAIS::sa_is(text, ALPHABET);
    // Remove the sentinel position (which is  n  ) from SA
    // At this point SA contains all indices [0..n], with sentinel 0-suffix at position 0.
    // Since 'text' had sentinel at n, SA[0] should be 'n'. Remove it:
    if (!sa.empty() && sa[0] == n) sa.erase(sa.begin());

    // Build rankAt (inverse SA)
    rankAt.assign(n, 0);
    for (int i = 0; i < n; ++i) rankAt[sa[i]] = i;

    // Kasai LCP
    lcp.assign(max(0, n - 1), 0);
    for (int i = 0, h = 0; i < n; ++i) {
      int r = rankAt[i];
      if (r == n - 1) { h = 0; continue; }
      int j = sa[r + 1];
      while (i + h < n && j + h < n && s[i + h] == s[j + h]) ++h;
      lcp[r] = h;
      if (h) --h;
    }

    // RMQ over LCP
    WANT_RMQ = build_rmq;
    if (WANT_RMQ && n >= 2) {
      int m = n - 1;                  // lcp size
      lg2.assign(m + 1, 0);
      for (int i = 2; i <= m; ++i) lg2[i] = lg2[i >> 1] + 1;
      int K = lg2[m] + 1;
      st.assign(K, vector<int>(m));
      st[0] = lcp;
      for (int k = 1; k < K; ++k) {
        int len = 1 << k;
        int half = len >> 1;
        for (int i = 0; i + len - 1 < m; ++i) {
          st[k][i] = min(st[k - 1][i], st[k - 1][i + half]);
        }
      }
    }
  }

  // --- Range minimum on LCP: min(lcp[L..R]) inclusive ---
  // Precondition: WANT_RMQ and 0 <= L <= R < n-1
  int rmq_lcp(int L, int R) const {
    if (L > R) return INT_MAX;
    int k = lg2[R - L + 1];
    return min(st[k][L], st[k][R - (1 << k) + 1]);
  }

  // --- LCP of suffixes starting at i and j in O(1) with RMQ; O(n) worst-case if RMQ disabled ---
  int get_lcp(int i, int j) const {
    if (i == j) return n - i;
    int ri = rankAt[i], rj = rankAt[j];
    if (ri > rj) swap(ri, rj);
    if (WANT_RMQ) return rmq_lcp(ri, rj - 1);
    // fallback (no RMQ): linear scan
    int len = 0;
    while (i + len < n && j + len < n && s[i + len] == s[j + len]) ++len;
    return len;
  }

  // --- Compare suffix s starting at sa[idx] with pattern 'pat' ---
  // returns: -1 if suffix < pat, 0 if prefix-equal and equal length, +1 if suffix > pat
  int compare_suffix_with_pattern(int idx, const string& pat) const {
    int i = sa[idx];
    int a = n - i;
    int b = (int)pat.size();
    int len = min(a, b);
    for (int k = 0; k < len; ++k) {
      if (s[i + k] < pat[k]) return -1;
      if (s[i + k] > pat[k]) return +1;
    }
    if (a == b) return 0;
    return (a < b) ? -1 : +1; // shorter suffix < longer pattern if prefix-equal
  }

  // --- Lower bound of pattern in SA (first suffix >= pattern) ---
  int lower_bound_pattern(const string& pat) const {
    int L = 0, R = n - 1, ans = n;
    while (L <= R) {
      int mid = (L + R) >> 1;
      int cmp = compare_suffix_with_pattern(mid, pat);
      if (cmp >= 0) { ans = mid; R = mid - 1; }
      else L = mid + 1;
    }
    return ans;
  }

  // --- Upper bound of pattern in SA (first suffix > pattern) ---
  int upper_bound_pattern(const string& pat) const {
    int L = 0, R = n - 1, ans = n;
    while (L <= R) {
      int mid = (L + R) >> 1;
      int cmp = compare_suffix_with_pattern(mid, pat);
      if (cmp > 0) { ans = mid; R = mid - 1; }
      else L = mid + 1;
    }
    return ans;
  }

  // --- [first, last) range of suffixes that start with 'pat' ---
  pair<int,int> find_occurrences(const string& pat) const {
    int L = lower_bound_pattern(pat);
    int R = upper_bound_pattern(pat);
    return {L, R}; // if L == R -> not found
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  string s;
  if (!(cin >> s)) return 0;

  // Build SA (with RMQ ready for O(1) LCP queries).
  SuffixArray SA(s, /*build_rmq=*/true);

  // Example output: print suffix array (0-based positions)
  for (int i = 0; i < (int)SA.sa.size(); ++i) {
    cout << SA.sa[i] << '\n';
  }
  return 0;
}
