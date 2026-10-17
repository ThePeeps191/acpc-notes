# Bitmask DP, Digit DP, SOS, Broken Profile

## Bitmask DP

**Assignment / ordering of n ≤ 20 items.** `dp[mask]` = best for having used the set `mask`; the next item index is often `popcount(mask)`.

```cpp
// TSP / Hamiltonian path: dp[mask][v] = min cost path visiting mask, ending at v. O(2^n n^2)
ll tsp(const vector<vector<ll>>& d) {      // returns min Hamiltonian cycle from 0
    int n = sz(d);
    vector<vector<ll>> dp(1 << n, vector<ll>(n, INF));
    dp[1][0] = 0;
    rep(mask, 1, 1 << n) rep(v, 0, n) {
        if (!(mask >> v & 1) || dp[mask][v] >= INF) continue;
        rep(u, 0, n) if (!(mask >> u & 1))
            dp[mask | 1 << u][u] = min(dp[mask | 1 << u][u], dp[mask][v] + d[v][u]);
    }
    ll best = INF;
    rep(v, 1, n) best = min(best, dp[(1 << n) - 1][v] + d[v][0]);
    return n == 1 ? 0 : best;
}
```

- **Assignment** (n workers ≤ 20 to n jobs): `dp[mask]` over assigned jobs, worker = popcount(mask). O(2^n·n). For n > 20 use Hungarian (§6.3).
- **Partition into groups** (min #groups each with weight ≤ W): `dp[mask] = (groups, current fill)` pairs, compare lexicographically. O(2^n·n).
- **Over submasks**: `dp[mask] = min over sub ⊆ mask of dp[mask ^ sub] + cost[sub]` → O(3^n); n ≤ 15–16.
- Memory: `2^20 × 20` ints = 80 MB; prefer `int` and drop dimensions.
- Bitmask over a **small dimension** of a grid (rows ≤ 10): dp[column][mask of current column].

## SOS DP (sum over subsets)

`F[mask] = Σ_{sub ⊆ mask} A[sub]` in O(n·2^n):

```cpp
void sos(vector<ll>& F, int n) {           // in place: F starts as A
    rep(i, 0, n) rep(mask, 0, 1 << n) if (mask >> i & 1) F[mask] += F[mask ^ (1 << i)];
}
// superset sums: condition !(mask >> i & 1), F[mask] += F[mask | 1 << i]
// inverse (Möbius): same loops with -=
```

Uses: count pairs with `a_i & a_j == 0` (count submasks of the complement), `a_i | a_j ⊆ x`, number of elements that are submasks of each x. AND/OR convolution = SOS transform, multiply pointwise, inverse transform.

## Digit DP

Count integers in [0, N] with a property depending on digits. Answer for [L, R] = f(R) − f(L − 1).

```cpp
// count x in [0, N] whose digit sum is divisible by K (K <= 100)
ll countDigitSum(ll N, int K) {
    if (N < 0) return 0;
    string s = to_string(N); int n = sz(s);
    // memo[pos][rem] for non-tight states only (tight path is unique, recomputed)
    vector<vector<ll>> memo(n, vector<ll>(K, -1));
    function<ll(int, int, bool)> go = [&](int pos, int rem, bool tight) -> ll {
        if (pos == n) return rem == 0;
        if (!tight && memo[pos][rem] != -1) return memo[pos][rem];
        int lim = tight ? s[pos] - '0' : 9;
        ll res = 0;
        rep(d, 0, lim + 1) res += go(pos + 1, (rem + d) % K, tight && d == lim);
        if (!tight) memo[pos][rem] = res;
        return res;
    };
    return go(0, 0, true);
}
```

Extra state flags as needed: `started` (to handle leading zeros, e.g. "no two adjacent equal digits"), `last digit`, `count of nonzero digits`, `rem mod m`. For "k-th number with property": compute counts per prefix choice and descend digit by digit.

## Broken profile DP (tilings)

Count domino tilings of an n × m grid (n ≤ 10): process cells in row-major order; mask = which of the next m cells are already covered.

```cpp
ll dominoTilings(int n, int m, ll mod) {          // m = width (small)
    vector<ll> dp(1 << m, 0); dp[0] = 1;
    rep(i, 0, n) rep(j, 0, m) {
        vector<ll> nd(1 << m, 0);
        rep(mask, 0, 1 << m) if (dp[mask]) {
            ll v = dp[mask];
            if (mask >> j & 1) { nd[mask ^ (1 << j)] = (nd[mask ^ (1 << j)] + v) % mod; continue; } // already covered
            nd[mask | (1 << j)] = (nd[mask | (1 << j)] + v) % mod;                  // vertical domino down
            if (j + 1 < m && !(mask >> (j + 1) & 1))                                // horizontal domino right
                nd[mask | (1 << (j + 1))] = (nd[mask | (1 << (j + 1))] + v) % mod;
        }
        dp = nd;
    }
    return dp[0];
}
```

Bit j of the mask refers to cell (i, j) when processing row i (set = covered by a vertical domino from above or a horizontal domino from the left). The same skeleton handles "place objects with local constraints" problems (e.g. minesweeper-like boards: keep the last two rows in the state when constraints reach distance 1).
