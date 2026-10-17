# Prefix Sums, Difference Arrays, Sweep Line, Contribution

## Prefix sums (1D/2D) and difference arrays

```cpp
// 1D: p[i] = a[0] + ... + a[i-1]; sum of a[l..r] = p[r+1] - p[l]
// 2D: P[i][j] = sum of a[0..i-1][0..j-1]
struct Pref2D {
    vector<vector<ll>> P;
    Pref2D(const vector<vector<ll>>& a) {
        int n = a.size(), m = a[0].size();
        P.assign(n + 1, vector<ll>(m + 1, 0));
        rep(i, 0, n) rep(j, 0, m) P[i + 1][j + 1] = a[i][j] + P[i][j + 1] + P[i + 1][j] - P[i][j];
    }
    ll query(int r1, int c1, int r2, int c2) {  // inclusive rectangle
        return P[r2 + 1][c2 + 1] - P[r1][c2 + 1] - P[r2 + 1][c1] + P[r1][c1];
    }
};
```

- **Difference array:** add v to a[l..r] ⇒ `d[l] += v; d[r+1] -= v;` then prefix-sum d once. 2D: `d[r1][c1] += v; d[r1][c2+1] -= v; d[r2+1][c1] -= v; d[r2+1][c2+1] += v`.
- Adding an arithmetic progression to ranges: apply difference twice (second-order difference).
- **Count subarrays with sum = k:** iterate prefix p, `ans += cnt[p - k]; cnt[p]++` (hashmap; start with `cnt[0] = 1`). Same for xor (`p ^ k`), and for "sum divisible by m" (`cnt[p mod m]`).
- **Longest subarray with sum ≤ k (non-negative values)**: two pointers. With negatives: prefix + monotonic stack / sorted prefix minima + binary search.
- **Prefix of a function over a cycle** (rotations, circular arrays): double the array, take windows of length n.

## Coordinate compression

```cpp
// sketch: vals = every coordinate that matters
vector<ll> xs = vals;                       // all coordinates that matter
sort(all(xs)); xs.erase(unique(all(xs)), xs.end());
auto id = [&](ll x) { return int(lower_bound(all(xs), x) - xs.begin()); };
```

For intervals on a compressed axis, remember that compressed index i represents the *point* xs[i]; the gap between xs[i] and xs[i+1] has length `xs[i+1] - xs[i]` (needed for union-of-intervals length / area).

## Sweep line

Turn geometry/intervals into **events sorted by coordinate**, process with a data structure.

- **Max overlapping intervals:** events (l, +1), (r, −1); sort; for closed intervals put +1 before −1 at equal coordinate.
- **Union length of intervals:** sort by l, merge greedily.
- **Area of union of rectangles:** sweep x; segment tree over compressed y storing "count covered > 0 length" (classic: node stores `cnt` and `covered len`, no push-down needed).
- **Pairs of intersecting segments / closest pair:** sweep with `set` ordered by current y (rarely needed at ACPC).
- **Offline queries:** sort queries together with updates by coordinate/time; answer each query when the sweep reaches it (e.g. "count points with x ≤ X and y ≤ Y" → sort by x, BIT on y).
- **Events at same coordinate:** decide the order explicitly (insertions before queries? removals first?) — this is the #1 sweep bug.

```cpp
// sketch: max number of simultaneously active closed intervals [l, r]
vector<pair<ll,int>> ev;
for (auto [l, r] : iv) { ev.push_back({l, -1}); ev.push_back({r, +1}); } // -1 = open sorts first
sort(all(ev));
int cur = 0, best = 0;
for (auto [x, t] : ev) { cur -= t; best = max(best, cur); }
```

## Contribution technique (sum over all subsets/subarrays/pairs)

Instead of enumerating structures, ask: **for each element, in how many structures does it contribute, and how much?**

- Σ over all subarrays of sum = Σ a_i · (i+1)·(n−i) (0-indexed).
- Σ over all subarrays of max: a_i is the max of subarrays with l ∈ (L_i, i], r ∈ [i, R_i) where L_i = previous index with value ≥ a_i (strict on one side, non-strict on the other to avoid double counting ties), R_i = next index with value > a_i. Contribution a_i·(i − L_i)·(R_i − i). Use monotonic stack (§2.3).
- Σ over pairs |a_i − a_j|: sort; a_i contributes `a_i·i − (sum of previous)`.
- Σ over all subsets of max: sort; a_i is the max in 2^i subsets.
- Σ of path lengths over all pairs in a tree: edge (parent, v) is used by sz[v]·(n − sz[v]) pairs.
- Expected value versions: same with probabilities (linearity of expectation, §4.5).

## Offline tricks summary

- Answer queries in a convenient order (sorted by right endpoint, by threshold, by time) and restore original order at the end — store the query index.
- Reverse time for deletions (DSU can't delete, but can add).
- Divide & conquer over time / CDQ: split operations, handle "left half updates → right half queries" with a sort + BIT.
