#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional> // For std::function in DP
#include <algorithm>  // For std::min, std::max, std::sort
#include <queue>        // For BFS in shortest_non_appearing
#include <limits>     // For std::numeric_limits

/**
 * @brief A contest-ready, expanded Suffix Automaton template.
 * * Implements all applications from cp-algorithms.com.
 * * Time Complexity: Build: O(N * log K) or O(N * K)
 * (O(N * log K) with std::map, O(N * K) with fixed-size array)
 */
struct SuffixAutomaton {
    
    /**
     * @brief Represents a single state in the Suffix Automaton.
     */
    struct State {
        int len;             // Length of the longest substring in this state
        int link;            // Suffix link to another state
        std::map<char, int> next; // Transitions on characters
        
        // --- Application fields ---
        
        /**
         * For counting occurrences: 1 for non-clones (end positions), 0 for clones.
         */
        long long terminal_occurrences = 0; 
        
        /**
         * For counting occurrences: Total occurrences, calculated by DP on suffix link tree.
         */
        long long total_occurrences = 0; 
        
        /**
         * For first/all occurrences: Smallest end position of substrings in this state.
         */
        int first_pos = -1; 
        
        /**
         * For DP on DAG: Number of distinct non-empty paths starting from this state.
         */
        long long distinct_paths = -1; 
        
        /**
         * For DP on DAG: Total length of all distinct non-empty paths starting from this state.
         */
        long long total_length = -1;   

        /**
         * For shortest non-appearing: Length of the shortest path from root to this state.
         */
        int shortest_path_len = -1;
        
        State() : len(0), link(-1) {}
    };

    std::vector<State> st; // Vector to store all states
    int sz;                // Current number of states
    int last;              // The state corresponding to the whole string processed so far
    int string_len = 0;    // Original string length
    std::vector<std::vector<int>> link_tree_adj; // Adjacency list for suffix link tree
    bool link_tree_built = false;

    /**
     * @brief Initializes the Suffix Automaton.
     */
    void init() {
        st.clear();
        st.resize(2); // Start with 2 to avoid resizing on first extend
        st[0].len = 0;
        st[0].link = -1;
        st[0].shortest_path_len = 0;
        sz = 1;
        last = 0;
        string_len = 0;
        link_tree_built = false;
        link_tree_adj.clear();
    }

    /**
     * @brief Default constructor.
     */
    SuffixAutomaton() {
        init();
    }

    /**
     * @brief Extends the automaton by adding a single character 'c'.
     */
    void extend(char c) {
        // Ensure capacity if needed
        if (sz >= st.size()) {
            st.resize(st.size() * 2);
        }

        int cur = sz++;
        st[cur].len = st[last].len + 1;
        st[cur].terminal_occurrences = 1; // Mark as end-position
        st[cur].first_pos = st[cur].len - 1; // First end pos is its own length
        
        int p = last;

        while (p != -1 && !st[p].next.count(c)) {
            st[p].next[c] = cur;
            p = st[p].link;
        }

        if (p == -1) {
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[cur].link = q;
            } else {
                int clone = sz++;
                if (sz >= st.size()) st.resize(st.size() * 2);

                st[clone] = st[q]; // Copy transitions, link, and first_pos
                st[clone].len = st[p].len + 1;
                st[clone].terminal_occurrences = 0; // Clones are not terminal
                
                while (p != -1 && st[p].next.count(c) && st[p].next[c] == q) {
                    st[p].next[c] = clone;
                    p = st[p].link;
                }
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }

    /**
     * @brief Builds the Suffix Automaton for the given string 's'.
     */
    void build(const std::string& s) {
        init();
        st.reserve(s.length() * 2 + 2); // Pre-allocate memory
        string_len = s.length();
        for (char c : s) {
            extend(c);
        }
    }

    /**
     * @brief Helper to get states sorted by length (for DP on link tree).
     */
    std::vector<int> get_states_by_len_order() {
        std::vector<std::vector<int>> states_by_len(string_len + 1);
        for (int v = 0; v < sz; ++v) {
            states_by_len[st[v].len].push_back(v);
        }
        
        std::vector<int> sorted_states;
        sorted_states.reserve(sz);
        for (int l = string_len; l >= 0; --l) {
            for (int v : states_by_len[l]) {
                sorted_states.push_back(v);
            }
        }
        return sorted_states;
    }

    /**
     * @brief Builds the adjacency list for the suffix link tree.
     */
    void build_suffix_link_tree() {
        if (link_tree_built) return;
        link_tree_adj.resize(sz);
        for (int v = 1; v < sz; ++v) {
            link_tree_adj[st[v].link].push_back(v);
        }
        link_tree_built = true;
    }
    
    /**
     * @brief Helper to find the state corresponding to a pattern.
     * @return State ID or -1 if not found.
     */
    int find_state(const std::string& p) {
        int cur = 0;
        for (char c : p) {
            if (!st[cur].next.count(c)) {
                return -1;
            }
            cur = st[cur].next[c];
        }
        return cur;
    }
    
    /**
     * @brief Recursive DP to calculate paths and total length on the DAG.
     * Fills `distinct_paths` and `total_length` for all states.
     */
    void dfs_dag_dps(int u) {
        if (st[u].distinct_paths != -1) return;

        st[u].distinct_paths = 0; // non-empty paths
        st[u].total_length = 0;

        for (auto const& [key, v] : st[u].next) {
            dfs_dag_dps(v);
            st[u].distinct_paths += (1 + st[v].distinct_paths);
            st[u].total_length += (1 + st[v].distinct_paths) + st[v].total_length;
        }
    }

    // --- APPLICATION 1: Check for occurrence ---
    /**
     * @brief Checks if the given pattern 'p' is a substring.
     */
    bool contains(const std::string& p) {
        return find_state(p) != -1;
    }
    
    // --- APPLICATION 2: Number of different substrings ---
    /**
     * @brief Counts the number of distinct substrings (O(N) method).
     */
    long long count_distinct_substrings() {
        long long total = 0;
        for (int v = 1; v < sz; ++v) {
            total += (st[v].len - st[st[v].link].len);
        }
        return total;
    }
    
    // --- APPLICATION 3: Total length of all different substrings ---
    /**
     * @brief Calculates the total length of all distinct substrings.
     */
    long long total_length_distinct_substrings() {
        // Clear old DP state
        for (int i = 0; i < sz; ++i) {
            st[i].distinct_paths = -1;
            st[i].total_length = -1;
        }
        dfs_dag_dps(0);
        return st[0].total_length;
    }

    // --- APPLICATION 4: Lexicographically k-th substring ---
    /**
     * @brief Finds the k-th lexicographically smallest substring.
     * @param k 1-indexed.
     * @return The k-th substring, or "" if k is out of bounds.
     */
    std::string find_kth_substring(long long k) {
        // Ensure DP is calculated
        if (st[0].distinct_paths == -1) {
            for (int i = 0; i < sz; ++i) {
                st[i].distinct_paths = -1;
                st[i].total_length = -1;
            }
            dfs_dag_dps(0);
        }

        if (k > st[0].distinct_paths) return ""; // k is too large

        std::string result = "";
        int u = 0;
        while (k > 0) {
            for (auto const& [key, v] : st[u].next) {
                long long paths_from_v = 1 + st[v].distinct_paths;
                if (k <= paths_from_v) {
                    result.push_back(key);
                    u = v;
                    k--; // Account for the 1-char path
                    break;
                } else {
                    k -= paths_from_v;
                }
            }
        }
        return result;
    }

    // --- APPLICATION 5: Smallest cyclic shift ---
    /**
     * @brief Finds the smallest cyclic shift of a string 's'.
     * This is a static method as it builds its own automaton.
     */
    static std::string find_smallest_cyclic_shift(const std::string& s) {
        if (s.empty()) return "";
        
        SuffixAutomaton sam_double;
        sam_double.build(s + s);
        
        int n = s.length();
        int u = 0;
        std::string result = "";
        
        for (int i = 0; i < n; i++) {
            if (sam_double.st[u].next.empty()) {
                // Should not happen if s is not empty
                break; 
            }
            // `std::map` guarantees iteration in alphabetical order
            auto it = sam_double.st[u].next.begin();
            result.push_back(it->first);
            u = it->second;
        }
        return result;
    }

    // --- APPLICATION 6: Number of occurrences ---
    /**
     * @brief Calculates `total_occurrences` for all states.
     * Must be called before `get_occurrences`.
     */
    void calculate_occurrences() {
        auto sorted_states = get_states_by_len_order();
        
        for (int v : sorted_states) {
            // Initialize total_occurrences with terminal_occurrences
            if (st[v].terminal_occurrences == 1) {
                st[v].total_occurrences = st[v].terminal_occurrences;
            } else {
                st[v].total_occurrences = 0;
            }
        }
        
        for (int v : sorted_states) {
            if (st[v].link != -1) {
                st[st[v].link].total_occurrences += st[v].total_occurrences;
            }
        }
    }

    /**
     * @brief Finds the number of occurrences of a pattern 'p'.
     * Must call `calculate_occurrences()` *before* using this.
     */
    long long get_occurrences(const std::string& p) {
        int v = find_state(p);
        return (v == -1) ? 0 : st[v].total_occurrences;
    }

    // --- APPLICATION 7: First occurrence position ---
    /**
     * @brief Finds the 0-indexed start position of the first occurrence of 'p'.
     * @return Start index, or -1 if not found.
     */
    int get_first_occurrence(const std::string& p) {
        int v = find_state(p);
        if (v == -1) return -1;
        return st[v].first_pos - p.length() + 1;
    }

    // --- APPLICATION 8: All occurrence positions ---
    /**
     * @brief Helper to find all end positions in the subtree of the link tree.
     */
    void dfs_all_occurrences(int u, std::vector<int>& results) {
        if (st[u].terminal_occurrences == 1) {
            results.push_back(st[u].first_pos);
        }
        for (int v : link_tree_adj[u]) {
            dfs_all_occurrences(v, results);
        }
    }

    /**
     * @brief Finds all 0-indexed start positions of a pattern 'p'.
     * @return A sorted vector of start positions.
     */
    std::vector<int> get_all_occurrences(const std::string& p) {
        int v = find_state(p);
        if (v == -1) return {};

        build_suffix_link_tree();
        
        std::vector<int> end_positions;
        dfs_all_occurrences(v, end_positions);
        
        std::vector<int> start_positions;
        start_positions.reserve(end_positions.size());
        for (int pos : end_positions) {
            start_positions.push_back(pos - p.length() + 1);
        }
        
        std::sort(start_positions.begin(), start_positions.end());
        return start_positions;
    }

    // --- APPLICATION 9: Shortest non-appearing string ---
    /**
     * @brief Finds the lexicographically smallest string not in the automaton.
     * @param alphabet The alphabet to consider.
     */
    std::string find_shortest_non_appearing(std::string alphabet = "abcdefghijklmnopqrstuvwxyz") {
        if (st[0].shortest_path_len == -1) { // Re-init if needed
             st[0].shortest_path_len = 0;
        }

        std::queue<int> q;
        q.push(0);
        std::string path = "";
        
        std::queue<std::string> path_q;
        path_q.push("");

        while (!q.empty()) {
            int u = q.front(); q.pop();
            path = path_q.front(); path_q.pop();

            for (char c : alphabet) {
                if (!st[u].next.count(c)) {
                    return path + c; // Found it
                } else {
                    int v = st[u].next[c];
                    // Use shortest_path_len as a 'visited' flag for BFS
                    if (st[v].shortest_path_len == -1) { 
                        st[v].shortest_path_len = st[u].shortest_path_len + 1;
                        q.push(v);
                        path_q.push(path + c);
                    }
                }
            }
        }
        return ""; // All strings in alphabet are present
    }

    // --- APPLICATION 10: Longest common substring of two strings ---
    /**
     * @brief Finds the LCS of two strings.
     * This is a static method.
     */
    static std::string lcs(const std::string& s1, const std::string& s2) {
        SuffixAutomaton sa;
        sa.build(s1);

        int l = 0, max_l = 0, best_pos = -1;
        int cur = 0;

        for (int i = 0; i < s2.length(); i++) {
            char c = s2[i];
            while (cur != 0 && !sa.st[cur].next.count(c)) {
                cur = sa.st[cur].link;
                l = sa.st[cur].len;
            }
            if (sa.st[cur].next.count(c)) {
                cur = sa.st[cur].next[c];
                l++;
            }
            if (l > max_l) {
                max_l = l;
                best_pos = i;
            }
        }
        if (best_pos == -1) return "";
        return s2.substr(best_pos - max_l + 1, max_l);
    }

    // --- APPLICATION 11: Largest common substring of multiple strings ---
    /**
     * @brief Finds the length of the LCS of multiple strings.
     * This is a static method.
     */
    static int lcs_multiple(const std::vector<std::string>& strings) {
        if (strings.empty()) return 0;

        // Build automaton on the first string
        SuffixAutomaton sa;
        sa.build(strings[0]);
        
        std::vector<int> min_lcs_for_state(sa.sz);
        for(int v=0; v<sa.sz; ++v) min_lcs_for_state[v] = sa.st[v].len;

        auto sorted_states = sa.get_states_by_len_order();

        for (size_t i = 1; i < strings.size(); ++i) {
            std::vector<int> current_lcs(sa.sz, 0);
            int l = 0, cur = 0;

            for (char c : strings[i]) {
                while (cur != 0 && !sa.st[cur].next.count(c)) {
                    cur = sa.st[cur].link;
                    l = sa.st[cur].len;
                }
                if (sa.st[cur].next.count(c)) {
                    cur = sa.st[cur].next[c];
                    l++;
                }
                current_lcs[cur] = std::max(current_lcs[cur], l);
            }

            // Propagate matches up the suffix link tree
            for (int v : sorted_states) {
                if (sa.st[v].link != -1) {
                    current_lcs[sa.st[v].link] = std::max(
                        current_lcs[sa.st[v].link],
                        std::min(current_lcs[v], sa.st[sa.st[v].link].len)
                    );
                }
            }
            
            // Update the minimum Lcs for each state
            for(int v=0; v<sa.sz; ++v) {
                min_lcs_for_state[v] = std::min(min_lcs_for_state[v], current_lcs[v]);
            }
        }

        // The answer is the max value in min_lcs_for_state
        int max_lcs = 0;
        for (int v = 0; v < sa.sz; ++v) {
            max_lcs = std::max(max_lcs, min_lcs_for_state[v]);
        }
        return max_lcs;
    }

};

// --- Example Usage ---
int main() {
    std::string s = "ababa";
    std::cout << "Building Suffix Automaton for: '" << s << "'" << std::endl;

    SuffixAutomaton sa;
    sa.build(s);

    std::cout << "\n--- Suffix Automaton Built ---" << std::endl;
    std::cout << "Number of states: " << sa.sz << std::endl;
    
    // App 1 & 2
    std::cout << "\n--- Distinct Substrings (App 2) & Contains (App 1) ---" << std::endl;
    std::cout << "Total distinct substrings (O(N) method): " 
              << sa.count_distinct_substrings() << std::endl;
    std::cout << "Contains 'aba': " << (sa.contains("aba") ? "Yes" : "No") << std::endl;
    std::cout << "Contains 'xyz': " << (sa.contains("xyz") ? "No" : "Yes") << std::endl;

    // App 3
    std::cout << "\n--- Total Length of Distinct Substrings (App 3) ---" << std::endl;
    std::cout << "Total length: " << sa.total_length_distinct_substrings() << std::endl;
    
    // App 4
    std::cout << "\n--- K-th Lexicographical Substring (App 4) ---" << std::endl;
    long long total_distinct = sa.count_distinct_substrings();
    std::cout << "Total distinct: " << total_distinct << std::endl;
    for (long long k = 1; k <= total_distinct; ++k) {
        std::cout << "k=" << k << ": " << sa.find_kth_substring(k) << std::endl;
    }
    
    // App 5
    std::cout << "\n--- Smallest Cyclic Shift (App 5) ---" << std::endl;
    std::string s_cyclic = "cabc";
    std::cout << "Smallest shift of '" << s_cyclic << "': " 
              << SuffixAutomaton::find_smallest_cyclic_shift(s_cyclic) << std::endl;

    // App 6, 7, 8
    std::cout << "\n--- Occurrences (App 6, 7, 8) ---" << std::endl;
    sa.calculate_occurrences();
    
    std::string p = "aba";
    std::cout << "Pattern '" << p << "':" << std::endl;
    std::cout << "  - Num Occurrences (App 6): " << sa.get_occurrences(p) << std::endl;
    std::cout << "  - First Occurrence (App 7): " << sa.get_first_occurrence(p) << std::endl;
    
    std::cout << "  - All Occurrences (App 8): ";
    std::vector<int> all_occ = sa.get_all_occurrences(p);
    for(int pos : all_occ) std::cout << pos << " ";
    std::cout << std::endl;

    // App 9
    std::cout << "\n--- Shortest Non-Appearing (App 9) ---" << std::endl;
    std::cout << "Shortest non-appearing (a-z): " << sa.find_shortest_non_appearing() << std::endl;

    // App 10
    std::cout << "\n--- LCS of Two Strings (App 10) ---" << std::endl;
    std::string s1 = "xabaxc";
    std::string s2 = "yabazc";
    std::cout << "LCS('" << s1 << "', '" << s2 << "'): " 
              << SuffixAutomaton::lcs(s1, s2) << std::endl;

    // App 11
    std::cout << "\n--- LCS of Multiple Strings (App 11) ---" << std::endl;
    std::vector<std::string> strings = {"ababa", "abaca", "abcba"};
    std::cout << "LCS length of {ababa, abaca, abcba}: " 
              << SuffixAutomaton::lcs_multiple(strings) << std::endl;

    return 0;
}