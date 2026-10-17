# Palindromes, Rotations, Tries, Aho–Corasick, Suffix Structures

## Manacher (all maximal palindromes in O(n))

```cpp
// [stress-tested] t = "#s0#s1#...#"; p[i] = radius in t (including centre).
// The palindrome centred at t[i] has length p[i] - 1 in s and starts at s index (i - p[i] + 1) / 2.
vector<int> manacher(const string& s) {
    string t = "#"; for (char c : s) { t += c; t += '#'; }
    int n = sz(t); vector<int> p(n, 0);
    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        int k = i > r ? 1 : min(p[l + r - i], r - i + 1);
        while (i - k >= 0 && i + k < n && t[i - k] == t[i + k]) ++k;
        p[i] = k--;
        if (i + k > r) { l = i - k; r = i + k; }
    }
    return p;
}
```

- Longest palindromic substring: max over i of `p[i] − 1`.
- Number of palindromic substrings: Σ_i ⌊p[i] / 2⌋.
- Is s[l..r] a palindrome: centre i = l + r + 1 in t; check `p[i] − 1 ≥ r − l + 1`.
- Palindromic **subsequences** are a different problem: interval DP (§4.1).
- Minimum insertions to make a palindrome = n − LPS (longest palindromic subsequence = LCS(s, reverse s)).

## Minimal rotation / canonical cyclic form

```cpp
// [stress-tested] index of the lexicographically smallest rotation, O(n)
int minRotation(const string& s) {
    int n = sz(s), i = 0, j = 1, k = 0;
    while (i < n && j < n && k < n) {
        char a = s[(i + k) % n], b = s[(j + k) % n];
        if (a == b) { ++k; continue; }
        if (a > b) i += k + 1; else j += k + 1;
        if (i == j) ++j;
        k = 0;
    }
    return min(i, j);
}
// canonical form up to rotation:      rotate(s, minRotation(s))
// up to rotation AND reflection:       min(canon(s), canon(reverse(s)))
```

Works for any sequence (vector of ints) — useful for necklaces, cyclic polygons (canonical edge/angle sequence), cycle comparison in graphs.

## Trie

```cpp
struct Trie {
    vector<array<int, 26>> nxt; vector<int> endCnt, passCnt;
    Trie() { newNode(); }
    int newNode() { nxt.push_back({}); nxt.back().fill(-1); endCnt.push_back(0); passCnt.push_back(0); return sz(nxt) - 1; }
    void insert(const string& s) {
        int v = 0; passCnt[0]++;
        for (char ch : s) { int c = ch - 'a'; if (nxt[v][c] < 0) { int u = newNode(); nxt[v][c] = u; } v = nxt[v][c]; passCnt[v]++; }
        endCnt[v]++;
    }
    int countPrefix(const string& s) {             // # inserted strings with prefix s
        int v = 0; for (char ch : s) { v = nxt[v][ch - 'a']; if (v < 0) return 0; } return passCnt[v];
    }
};
```

**Binary trie** over bits (high to low) for max xor with a set: at each level go to the child with the opposite bit if it exists. Store counts to support deletion and "count values with x ^ v < k".

## Aho–Corasick (many patterns in one text)

```cpp
// [stress-tested] lowercase. After build(): cnt[state] = #patterns that end at state (incl. via links).
struct AhoCorasick {
    vector<array<int, 26>> go; vector<int> link, cnt, order;
    AhoCorasick() { add(); }
    int add() { go.push_back({}); go.back().fill(-1); link.push_back(0); cnt.push_back(0); return sz(go) - 1; }
    int insert(const string& s) {                  // returns terminal state of s
        int v = 0;
        for (char ch : s) { int c = ch - 'a'; if (go[v][c] < 0) { int u = add(); go[v][c] = u; } v = go[v][c]; }
        cnt[v]++; return v;
    }
    void build() {
        queue<int> q; order.clear();
        rep(c, 0, 26) { if (go[0][c] < 0) go[0][c] = 0; else { link[go[0][c]] = 0; q.push(go[0][c]); } }
        while (!q.empty()) {
            int v = q.front(); q.pop(); order.push_back(v);
            cnt[v] += cnt[link[v]];
            rep(c, 0, 26) {
                int u = go[v][c];
                if (u < 0) go[v][c] = go[link[v]][c];
                else { link[u] = go[link[v]][c]; q.push(u); }
            }
        }
    }
    ll countMatches(const string& text) {          // total pattern occurrences in text
        ll res = 0; int v = 0;
        for (char ch : text) { v = go[v][ch - 'a']; res += cnt[v]; }
        return res;
    }
};
```

- Occurrences **per pattern**: count visits per state while scanning, then push counts up the suffix-link tree in reverse BFS `order` (`visits[link[v]] += visits[v]`), read at each pattern's terminal state.
- DP over the automaton: "count strings of length n containing none of the patterns" = dp over (length, state) avoiding states with cnt > 0.

## Suffix array + LCP

```cpp
// [stress-tested] sa[0] = n (empty suffix), sa[1..n] = suffixes in sorted order.
// lcp[i] = LCP(suffix sa[i-1], suffix sa[i]) for i >= 1. Characters must be > 0.
struct SuffixArray {
    vector<int> sa, lcp;
    SuffixArray(string s, int lim = 256) {
        s.push_back(0); int n = sz(s), k = 0, a, b;
        vector<int> x(all(s)), y(n), ws(max(n, lim));
        sa = lcp = y; iota(all(sa), 0);
        for (int j = 0, p = 0; p < n; j = max(1, j * 2), lim = p) {
            p = j; iota(all(y), n - j);
            rep(i, 0, n) if (sa[i] >= j) y[p++] = sa[i] - j;
            fill(all(ws), 0);
            rep(i, 0, n) ws[x[i]]++;
            rep(i, 1, lim) ws[i] += ws[i - 1];
            for (int i = n; i--;) sa[--ws[x[y[i]]]] = y[i];
            swap(x, y); p = 1; x[sa[0]] = 0;
            rep(i, 1, n) { a = sa[i - 1]; b = sa[i]; x[b] = (y[a] == y[b] && y[a + j] == y[b + j]) ? p - 1 : p++; }
        }
        for (int i = 0, j; i < n - 1; lcp[x[i++]] = k)
            for (k && k--, j = sa[x[i] - 1]; s[i + k] == s[j + k]; k++);
    }
};
```

- **Distinct substrings** = n(n+1)/2 − Σ lcp.
- **LCP of any two suffixes** = range-min of lcp between their ranks (sparse table, §3.4).
- **Longest repeated substring** = max lcp. **Longest common substring of A and B**: SA of `A + '#' + B` (separator smaller than letters but > 0, e.g. '#'), max lcp over adjacent pairs from different sides.
- **k-th distinct substring**: walk sa in order, each suffix contributes (n − sa[i] − lcp[i]) new substrings.
- Pattern search: binary search over sa comparing with the pattern, O(m log n).

## Suffix automaton (when you need counts of occurrences of every substring)

```cpp
// [stress-tested] states <= 2n. Distinct substrings = sum over v != 0 of len[v] - len[link[v]].
struct SAM {
    struct St { int len, link; map<char, int> next; };
    vector<St> st; int last = 0;
    SAM() { st.push_back({0, -1, {}}); }
    void extend(char c) {
        int cur = sz(st); st.push_back({st[last].len + 1, 0, {}});
        int p = last;
        while (p != -1 && !st[p].next.count(c)) { st[p].next[c] = cur; p = st[p].link; }
        if (p != -1) {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) st[cur].link = q;
            else {
                int clone = sz(st); St cl = st[q]; cl.len = st[p].len + 1; st.push_back(cl);
                while (p != -1 && st[p].next.count(c) && st[p].next[c] == q) { st[p].next[c] = clone; p = st[p].link; }
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }
};
```

Occurrences of each state's substrings: `occ[cur] = 1` for non-clone states, then add occ[v] to occ[link[v]] in decreasing order of len. A string P occurs in s iff walking `next` from state 0 along P never fails.
