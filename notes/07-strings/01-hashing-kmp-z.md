# Hashing, Prefix Function (KMP), Z-Function

## Polynomial hashing (mod 2^61 − 1, random base)

```cpp
// [stress-tested] O(1) substring hash after O(n) preprocessing. Collision prob ~ n^2 / 2^61.
struct Hasher {
    static const ull M = (1ULL << 61) - 1;
    static ull mul(ull a, ull b) {
        __uint128_t c = (__uint128_t)a * b;
        ull r = (ull)(c & M) + (ull)(c >> 61);
        return r >= M ? r - M : r;
    }
    static ull base() { static ull B = mt19937_64(chrono::steady_clock::now().time_since_epoch().count())() % (M - 1000) + 500; return B; }
    vector<ull> h, p;
    Hasher(const string& s) : h(sz(s) + 1, 0), p(sz(s) + 1, 1) {
        ull B = base();
        rep(i, 0, sz(s)) { h[i + 1] = (mul(h[i], B) + (unsigned char)s[i]) % M; p[i + 1] = mul(p[i], B); }
    }
    ull get(int l, int r) const {                  // hash of s[l, r)
        ull v = h[r] + M - mul(h[l], p[r - l]);
        return v >= M ? v - M : v;
    }
    // hash of concatenation A + B given hash(A), hash(B), len(B): mul(hA, p^lenB) + hB  (mod M)
};
```

Uses:
- **Compare substrings** in O(1); **LCP of two suffixes** in O(log n) by binary search on length → suffix comparison → sort suffixes/rotations with a custom comparator in O(n log² n).
- **Longest repeated substring / longest common substring of k strings**: binary search the length, put all hashes of that length in a hash set.
- **Palindrome check** of s[l, r): hash of s vs hash of reverse(s) on the mirrored range.
- **Canonical forms** for sets of strings: hash of the minimal rotation (§8.2) for "equal up to rotation" (ACPC 2020 *Password Rotation*: insert min-rotation of s and of reverse(s)).
- Hashing sequences of ints: use values + 1 (avoid 0) and the same formula; for multisets use sum/xor of random 64-bit values per element (§2.5).
- Anti-hack is irrelevant on Kattis, but **never use base 31/mod 2^64 with natural overflow** on adversarial tests (Thue–Morse strings break it).

## Prefix function and KMP

`pi[i]` = length of the longest proper border (prefix = suffix) of s[0..i].

```cpp
// [stress-tested]
vector<int> prefixFunction(const string& s) {
    int n = sz(s); vector<int> pi(n, 0);
    rep(i, 1, n) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) j = pi[j - 1];
        if (s[i] == s[j]) ++j;
        pi[i] = j;
    }
    return pi;
}
vector<int> kmpFind(const string& text, const string& pat) {    // start indices of occurrences
    vector<int> pi = prefixFunction(pat), res; int j = 0, m = sz(pat);
    if (m == 0) return res;
    rep(i, 0, sz(text)) {
        while (j > 0 && text[i] != pat[j]) j = pi[j - 1];
        if (text[i] == pat[j]) ++j;
        if (j == m) { res.push_back(i - m + 1); j = pi[j - 1]; }
    }
    return res;
}
```

- **Smallest period** of s: p = n − pi[n−1]; s is a repetition of its prefix of length p iff n % p == 0.
- All borders: n, pi[n−1], pi[pi[n−1]−1], … (chain).
- **Count occurrences of each prefix** in s: `cnt[pi[i]]++`, then propagate `cnt[pi[i−1]] += cnt[i]` from long to short, plus 1 each.
- **KMP automaton**: `go[state][c]` for building DP over strings that must avoid/contain a pattern ("count strings of length n not containing P": dp over (length, automaton state) — matrix power for huge n).
- Rotation check: t is a rotation of s ⇔ |s| = |t| and t occurs in s + s.

## Z-function

`z[i]` = length of the longest common prefix of s and s[i..].

```cpp
// [stress-tested]
vector<int> zFunction(const string& s) {
    int n = sz(s); vector<int> z(n, 0);
    if (n) z[0] = n;
    for (int i = 1, l = 0, r = 0; i < n; ++i) {
        if (i < r) z[i] = min(r - i, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) ++z[i];
        if (i + z[i] > r) { l = i; r = i + z[i]; }
    }
    return z;
}
```

- Occurrences of P in T: z-function of `P + '#' + T`, positions with z ≥ |P|.
- For each suffix of T, the longest prefix of P that starts there: same concatenation trick.
- Number of distinct substrings in O(n²) by appending characters and using z on the reversed prefix.
- z and pi are interconvertible; use whichever is easier to reason about.
