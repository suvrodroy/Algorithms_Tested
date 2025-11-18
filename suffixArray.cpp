#include <bits/stdc++.h>
using namespace std;

namespace SAIS {

  // SA array
  // isLType[i] = true if L-type, false if S-type
  // lmsPositions = indices of LMS positions
  static void induced_sort(const vector<int>& text, int alphabet,
                           vector<int>& sa,
                           const vector<bool>& isLType,
                           const vector<int>& lmsPositions) {
    const int n = (int)text.size();
    vector<int> bucketStart(alphabet, 0), bucketEnd(alphabet, 0); // bucket heads/tails

    for (int c : text) ++bucketEnd[c];
    partial_sum(bucketEnd.begin(), bucketEnd.end(), bucketEnd.begin());
    bucketStart[0] = 0;
    for (int c = 1; c < alphabet; ++c) bucketStart[c] = bucketEnd[c-1];

    fill(sa.begin(), sa.end(), -1);

    vector<int> tail = bucketEnd; // moving ends for S insertions
    for (int i = (int)lmsPositions.size() - 1; i >= 0; --i) {
      int p = lmsPositions[i];
      sa[--tail[text[p]]] = p;
    }

    vector<int> head = bucketStart; // moving heads for L insertions
    for (int idx = 0; idx < n; ++idx) {
      int p = sa[idx];
      if (p > 0 && isLType[p - 1]) {
        sa[head[text[p - 1]]++] = p - 1;
      }
    }

    vector<int> count(alphabet, 0); // character counts
    for (int c : text) ++count[c];
    for (int c = 0, acc = 0; c < alphabet; ++c) { acc += count[c]; bucketEnd[c] = acc; }

    tail = bucketEnd;
    for (int idx = n - 1; idx >= 0; --idx) {
      int p = sa[idx];
      if (p > 0 && !isLType[p - 1]) {
        sa[--tail[text[p - 1]]] = p - 1;
      }
    }
  }

  // sa: suffix array
  // isLType: L/S types per position (true=L, false=S)
  // lmsPositions: LMS indices
  // nameOfPos[i]: name id for LMS substring starting at i (or -1)
  // reduced: reduced string of LMS names
  // saLMS: LMS positions in lexicographic order
  static vector<int> sa_is(const vector<int>& text, int alphabet) {
    const int n = (int)text.size();
    vector<int> sa(n, -1);                // suffix array (includes sentinel suffix)
    vector<bool> isLType(n, false);       // types per index
    vector<int> lmsPositions;             // LMS indices

    for (int i = n - 2; i >= 0; --i) {
      isLType[i] = (text[i] > text[i + 1]) || (text[i] == text[i + 1] && isLType[i + 1]);
      if (isLType[i] && !isLType[i + 1]) lmsPositions.push_back(i + 1);
    }
    reverse(lmsPositions.begin(), lmsPositions.end());

    induced_sort(text, alphabet, sa, isLType, lmsPositions);

    vector<int> sortedLMS;                // LMS in SA order
    sortedLMS.reserve(lmsPositions.size());
    for (int p : sa) if (p >= 1 && !isLType[p] && isLType[p - 1]) sortedLMS.push_back(p);

    vector<int> nameOfPos(n, -1);         // names for LMS starts
    int currentName = 0;
    nameOfPos[sortedLMS[0]] = 0;

    auto is_lms = [&](int i) -> bool { return i > 0 && !isLType[i] && isLType[i - 1]; };
    auto equal_lms_substring = [&](int a, int b) -> bool {
      if (text[a] != text[b]) return false;
      int i = a + 1, j = b + 1;
      while (true) {
        bool aLMS = is_lms(i);
        bool bLMS = is_lms(j);
        if (text[i] != text[j]) return false;
        if (aLMS || bLMS) return aLMS && bLMS;
        ++i; ++j;
      }
    };

    for (int k = 1; k < (int)sortedLMS.size(); ++k) {
      int prev = sortedLMS[k - 1];
      int cur  = sortedLMS[k];
      if (!equal_lms_substring(prev, cur)) ++currentName;
      nameOfPos[cur] = currentName;
    }

    int m = (int)lmsPositions.size();
    vector<int> reduced(m);               // reduced string of LMS names
    for (int i = 0; i < m; ++i) reduced[i] = nameOfPos[lmsPositions[i]];

    vector<int> saLMS;                    // LMS positions in correct order
    if (currentName + 1 == m) {
      saLMS.resize(m);
      iota(saLMS.begin(), saLMS.end(), 0);
      sort(saLMS.begin(), saLMS.end(), [&](int a, int b){ return reduced[a] < reduced[b]; });
      for (int i = 0; i < m; ++i) saLMS[i] = lmsPositions[saLMS[i]];
    } else {
      vector<int> reducedText(m + 1);     // reduced text + sentinel
      for (int i = 0; i < m; ++i) reducedText[i] = reduced[i] + 1;
      reducedText[m] = 0;
      auto saReduced = sa_is(reducedText, currentName + 2);
      saReduced.erase(saReduced.begin());
      saLMS.resize(m);
      for (int i = 0; i < m; ++i) saLMS[i] = lmsPositions[saReduced[i]];
    }

    induced_sort(text, alphabet, sa, isLType, saLMS);
    return sa;
  }

} // namespace SAIS

struct SuffixArray {
  int n = 0;                       // string length
  string s;                        // original string
  vector<int> sa;                  // sa[i] = start index of i-th suffix
  vector<int> rankAt;              // rankAt[pos] = rank of suffix at pos
  vector<int> lcp;                 // lcp[i] = LCP(sa[i], sa[i+1])
  vector<vector<int>> st;          // sparse table over lcp
  vector<int> lg2;                 // floor log2
  bool WANT_RMQ = true;            // toggle RMQ build

  SuffixArray() {}
  explicit SuffixArray(const string& str, bool build_rmq = true) { build(str, build_rmq); }

  void build(const string& str, bool build_rmq = true) {
    s = str; n = (int)s.size();

    const int ALPHABET = 257;      // 256 bytes + sentinel
    vector<int> text(n + 1, 0);    // integer-mapped text + sentinel 0
    for (int i = 0; i < n; ++i) text[i] = (unsigned char)s[i] + 1;
    text[n] = 0;

    sa = SAIS::sa_is(text, ALPHABET);   // SA with sentinel suffix
    if (!sa.empty() && sa[0] == n) sa.erase(sa.begin());

    rankAt.assign(n, 0);            // inverse SA
    for (int i = 0; i < n; ++i) rankAt[sa[i]] = i;

    lcp.assign(max(0, n - 1), 0);   // Kasai LCP
    for (int i = 0, h = 0; i < n; ++i) {
      int r = rankAt[i];
      if (r == n - 1) { h = 0; continue; }
      int j = sa[r + 1];
      while (i + h < n && j + h < n && s[i + h] == s[j + h]) ++h;
      lcp[r] = h;
      if (h) --h;
    }

    WANT_RMQ = build_rmq;
    if (WANT_RMQ && n >= 2) {
      int m = n - 1;                // lcp size
      lg2.assign(m + 1, 0);
      for (int i = 2; i <= m; ++i) lg2[i] = lg2[i >> 1] + 1;

      int K = lg2[m] + 1;
      st.assign(K, vector<int>(m));
      st[0] = lcp;
      for (int k = 1; k < K; ++k) {
        int len = 1 << k, half = len >> 1;
        for (int i = 0; i + len - 1 < m; ++i)
          st[k][i] = min(st[k - 1][i], st[k - 1][i + half]);
      }
    }
  }

  // Range minimum on lcp[L..R] inclusive
  int rmq_lcp(int L, int R) const {
    if (L > R) return INT_MAX;
    int k = lg2[R - L + 1];
    return min(st[k][L], st[k][R - (1 << k) + 1]);
  }

  // LCP of suffixes starting at i and j
  // returns length of the longest common prefix of s[i..] and s[j..]
  int get_lcp(int i, int j) const {
    if (i == j) return n - i;
    int ri = rankAt[i], rj = rankAt[j];
    if (ri > rj) swap(ri, rj);
    if (WANT_RMQ) return rmq_lcp(ri, rj - 1);
    int len = 0; // fallback without RMQ
    while (i + len < n && j + len < n && s[i + len] == s[j + len]) ++len;
    return len;
  }

  // Compare suffix s[sa[idx]..] with pattern 'pat'
  // returns: -1 if suffix < pat, 0 if equal, +1 if suffix > pat
  int compare_suffix_with_pattern(int idx, const string& pat) const {
    int i = sa[idx];
    int a = n - i, b = (int)pat.size();
    int len = min(a, b);
    for (int k = 0; k < len; ++k) {
      if (s[i + k] < pat[k]) return -1;
      if (s[i + k] > pat[k]) return +1;
    }
    if (a == b) return 0;
    return (a < b) ? -1 : +1;
  }

  // First suffix >= pattern
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

  // First suffix > pattern
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

  // Half-open range [L, R) of suffixes that start with 'pat'
  pair<int,int> find_occurrences(const string& pat) const {
    int L = lower_bound_pattern(pat);
    int R = upper_bound_pattern(pat);
    return {L, R};
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  string s;
  if (!(cin >> s)) return 0;

  SuffixArray SA(s, /*build_rmq=*/true);

  for (int i = 0; i < (int)SA.sa.size(); ++i) {
    cout << SA.sa[i] << '\n';
  }
  return 0;
}
