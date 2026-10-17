# DP Design and Classic Patterns

## Designing a DP

1. **State** = the minimum information about the past that determines the future. Write it as a sentence: "dp[i][j] = max score using the first i items with j capacity used".
2. **Transition**: what is the last decision? (last item taken or not, where the last segment starts, which colour the last node got.)
3. **Order**: compute states after everything they depend on (increasing i; increasing interval length; post-order on a tree; topological order on a DAG). If there are cycles in the dependency, it's a shortest path / system of equations, not a DP.
4. **Count states × transitions** — reduce with prefix sums/min, monotonic deque, data structures, or a smarter state.
5. Base cases, answer location, modulo, overflow, ±INF for impossible states (use `INF/2` so additions don't overflow).

Tricks: **memoized recursion** is the fastest way to a correct DP when the order is unclear. **Rolling arrays** (`dp[i & 1]`) cut memory. **Sort first** if items can be processed in any order (by deadline, by weight) — then a knapsack-like DP over the sorted order is valid. To **reconstruct** the solution, store the argmax choice per state or re-derive by checking which transition achieves the optimum.

## Knapsack family

```cpp
// 0/1 knapsack: max value with capacity W  (iterate capacity DOWN)
ll knap01(const vector<int>& w, const vector<ll>& v, int W) {
    vector<ll> dp(W + 1, 0);
    rep(i, 0, sz(w)) for (int c = W; c >= w[i]; --c) dp[c] = max(dp[c], dp[c - w[i]] + v[i]);
    return dp[W];
}
// unbounded: iterate capacity UP.  count ways to make sum (order-independent): items outer loop
ll countWays(const vector<int>& coins, int S, ll mod) {
    vector<ll> dp(S + 1, 0); dp[0] = 1;
    for (int c : coins) rep(s, c, S + 1) dp[s] = (dp[s] + dp[s - c]) % mod;
    return dp[S];   // swap loops (sum outer, coins inner) -> counts ordered sequences
}
```

- **Bounded** (item i available k_i times): split k into 1, 2, 4, …, remainder → O(W Σ log k_i). Or monotonic deque per residue class mod w → O(nW).
- **Values small, weights huge:** dp[value] = min weight.
- **Subset sum reachability**: bitset (§2.6). Σ w = n ⇒ ≤ √(2n) distinct weights.
- Min items to reach exact sum: dp[s] = min(dp[s − c] + 1).
- **Huge sums with few coin types (Frobenius / reachability for all large X):** Dijkstra on residues modulo the smallest coin (§5.1, ACPC 2022 *Braaaains*).

## Sequences

```cpp
// LIS length, O(n log n). For non-decreasing use upper_bound.
int lis(const vector<ll>& a) {
    vector<ll> d;                       // d[k] = min tail of an increasing subsequence of length k+1
    for (ll x : a) {
        auto it = lower_bound(all(d), x);
        if (it == d.end()) d.push_back(x); else *it = x;
    }
    return sz(d);
}
```

To reconstruct LIS: store for each i the position `pos[i]` it took in d and `prev[i]` = last index placed at `pos[i]−1`. Count of LIS: BIT over values storing (best length, count).

- **LCS** O(nm): `dp[i][j] = s[i−1]==t[j−1] ? dp[i−1][j−1]+1 : max(dp[i−1][j], dp[i][j−1])`.
- **Edit distance**: `dp[i][j] = min(dp[i−1][j]+1, dp[i][j−1]+1, dp[i−1][j−1] + (s[i−1]!=t[j−1]))`.
- **Interleaving** s = merge of s1, s2 keeping order (ACPC 2018 *Homework*): `ok[i][j]` = first i+j chars of s are an interleaving of s1[0..i), s2[0..j): `ok[i][j] = (ok[i−1][j] && s1[i−1]==s[i+j−1]) || (ok[i][j−1] && s2[j−1]==s[i+j−1])`. 25·10^6 booleans is fine; use `vector<char>` or bitset rows.
- **Count distinct subsequences**: `dp[i] = 2·dp[i−1] − dp[last[c]−1]` where last[c] = previous occurrence of the same char.
- Max subarray sum (Kadane): `cur = max(x, cur + x)`.

## Interval DP

`dp[l][r]` from smaller intervals; iterate `len` from 1 to n. O(n³) with a split point, O(n²) without.

```cpp
// number of palindromic subsequences (as index sets, non-empty) mod p — ACPC 2016 Palindromic Naming
ll countPalSubseq(const string& s, ll mod) {
    int n = sz(s);
    vector<vector<ll>> dp(n, vector<ll>(n, 0));
    for (int l = n - 1; l >= 0; --l) {
        dp[l][l] = 1;
        rep(r, l + 1, n) {
            ll in = (l + 1 <= r - 1) ? dp[l + 1][r - 1] : 0;
            dp[l][r] = (dp[l + 1][r] + dp[l][r - 1] - in + mod) % mod;  // inclusion–exclusion
            if (s[l] == s[r]) dp[l][r] = (dp[l][r] + in + 1) % mod;     // pair s[l], s[r] around any inner one or none
        }
    }
    return dp[0][n - 1];
}
```

- **Split point**: `dp[l][r] = min over l ≤ k < r of dp[l][k] + dp[k+1][r] + cost(l, r)` (matrix chain, merging stones, optimal BST; Knuth optimisation §4.4 if cost is quadrangle-monotone).
- **Expressions with + and −, choose parenthesisation to maximise** (ACPC 2022 *Maximal Parentheticals*): keep **both max and min** of every sub-expression, since `a − (b)` is maximised by minimising b. With multiplication by negatives also keep both (that problem also lets parentheses create unary minus + implicit multiplication — enumerate every rule combination).
- **Remove/merge adjacent elements** ("zuma", "remove all with cost"): dp over [l, r] plus an extra dimension for "how many equal elements are glued to the right end".
- **Convex polygon DP** (triangulations, ACPC 2018 *Trimming Convex Polygons*): vertices 0..n−1 in order; choosing a sub-polygon = choosing a subsequence of vertices; its area is a fan of triangles from the first chosen vertex. dp over (first kept vertex fixed, last kept vertex) with transitions adding one triangle: O(n³).
- **Circular** intervals: double the array and take windows of length n, or fix the element at position 0.

## Grid, DAG, partition DPs

- Grid paths with obstacles: `dp[i][j] = dp[i−1][j] + dp[i][j−1]`. Count paths avoiding k special cells (n, m large): sort cells, inclusion–exclusion with C(dx+dy, dx) — O(k²).
- Grid with moves S/E/SE and accumulating state (ACPC 2024 *Alpine Agility*): row-major order works because all moves go forward.
- DAG longest path / count paths: topological order (§5.2).
- **Partition into segments**: `dp[i] = min_j dp[j] + cost(j, i)`; with k segments add a dimension → D&C / CHT optimisations (§4.4) or the λ-trick.
- **State "was my neighbour already processed"**: when the order of operations on a line matters only locally (destroying buildings that damage neighbours, ACPC 2016 *City Destruction*), dp over position i with a flag for whether i−1 is destroyed before or after i.

## Counting DPs (mod p)

- dp over (position, last value) with prefix sums over last value → O(n·V).
- "Number of sequences with sum S": dp over sum; with bounded parts use prefix sums to make each step O(1).
- Avoid double counting: define a **canonical** construction (e.g. always extend from the leftmost undecided position, or count by "first time the event happens").
- When the DP counts objects up to symmetry, use Burnside (§7.3) instead.
