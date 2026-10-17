# DP Optimisations

| recurrence shape | technique | complexity |
|---|---|---|
| `dp[i] = min_j dp[j] + f(j) + g(i)` with j in a sliding window | monotonic deque (§2.3) | O(n) |
| `dp[i] = min_j (a_j · x_i + b_j)` (+ terms of i) | convex hull trick / Li Chao | O(n log n) |
| `dp[k][i] = min_{j<i} dp[k−1][j] + C(j, i)`, opt(i) monotone | divide & conquer | O(k n log n) |
| `dp[i][j] = min_{i≤k<j} dp[i][k] + dp[k+1][j] + C(i, j)` | Knuth (opt[i][j−1] ≤ opt[i][j] ≤ opt[i+1][j]) | O(n²) |
| "exactly k groups" and answer convex in k | Aliens trick (λ-penalty, binary search) | O(n log n log C) |
| `dp[x]` is convex piecewise linear in a value x | slope trick (heaps of breakpoints) | O(n log n) |

**Monge / quadrangle inequality** `C(a, c) + C(b, d) ≤ C(a, d) + C(b, c)` for a ≤ b ≤ c ≤ d implies monotone opt (both D&C and Knuth). Typical Monge costs: (sum of segment)², sum of |x − median| over a segment, number of pairs inside a segment. When unsure, stress-test opt monotonicity with a brute force.

## Convex hull trick (any insertion/query order)

```cpp
// [stress-tested] max of k*x + m. For min: add(-k, -m) and negate query.
struct Line {
    mutable ll k, m, p;
    bool operator<(const Line& o) const { return k < o.k; }
    bool operator<(ll x) const { return p < x; }
};
struct LineContainer : multiset<Line, less<>> {
    static const ll inf = LLONG_MAX;
    ll div(ll a, ll b) { return a / b - ((a ^ b) < 0 && a % b); }  // floor
    bool isect(iterator x, iterator y) {
        if (y == end()) return x->p = inf, 0;
        if (x->k == y->k) x->p = x->m > y->m ? inf : -inf;
        else x->p = div(y->m - x->m, x->k - y->k);
        return x->p >= y->p;
    }
    void add(ll k, ll m) {
        auto z = insert({k, m, 0}), y = z++, x = y;
        while (isect(y, z)) z = erase(z);
        if (x != begin() && isect(--x, y)) isect(x, y = erase(y));
        while ((y = x) != begin() && (--x)->p >= y->p) isect(x, erase(y));
    }
    ll query(ll x) {
        auto l = *lower_bound(x);
        return l.k * x + l.m;
    }
};
```

Watch overflow: |k·x| must fit in `ll`. Example: `dp[i] = min_j dp[j] + (p_i − p_j)²` = `p_i² + min_j (−2p_j·p_i + dp[j] + p_j²)` → line with slope `−2p_j`, intercept `dp[j] + p_j²`, query at `p_i`.

## Li Chao tree (min, over a known set of query x's)

Also supports adding a line only on an x-range (descend to O(log n) nodes and insert there).

```cpp
// [stress-tested]
struct LiChao {
    struct L { ll k, m; ll eval(ll x) const { return k * x + m; } };
    vector<ll> xs; vector<L> t; int n;
    LiChao(vector<ll> pts) : xs(pts) {
        sort(all(xs)); xs.erase(unique(all(xs)), xs.end());
        n = sz(xs); t.assign(4 * n, {0, INF});
    }
    void add(ll k, ll m) { add({k, m}, 1, 0, n - 1); }
    void add(L nw, int x, int l, int r) {
        int mid = (l + r) / 2;
        bool lef = nw.eval(xs[l]) < t[x].eval(xs[l]);
        bool md = nw.eval(xs[mid]) < t[x].eval(xs[mid]);
        if (md) swap(t[x], nw);
        if (l == r) return;
        if (lef != md) add(nw, 2 * x, l, mid); else add(nw, 2 * x + 1, mid + 1, r);
    }
    ll query(ll qx) {                    // qx must be one of pts
        int i = lower_bound(all(xs), qx) - xs.begin(), x = 1, l = 0, r = n - 1;
        ll res = INF;
        while (true) {
            res = min(res, t[x].eval(qx));
            if (l == r) return res;
            int mid = (l + r) / 2;
            if (i <= mid) { x = 2 * x; r = mid; } else { x = 2 * x + 1; l = mid + 1; }
        }
    }
};
```

The empty line `{0, INF}` evaluates to INF; don't add lines with huge slopes where `k*x` overflows.

## Divide & conquer optimisation

```cpp
// [stress-tested] split a[0..n) into exactly K contiguous groups minimising sum of cost(j, i) for
// group = a[j..i). cost must satisfy the quadrangle inequality.
ll partitionDC(int n, int K, const function<ll(int, int)>& cost) {
    vector<ll> prv(n + 1, INF), cur(n + 1, INF);
    prv[0] = 0;
    function<void(int, int, int, int)> solve = [&](int l, int r, int optl, int optr) {
        if (l > r) return;
        int mid = (l + r) / 2, opt = optl;
        ll best = INF;
        for (int j = optl; j <= min(mid - 1, optr); ++j)
            if (prv[j] < INF && prv[j] + cost(j, mid) < best) { best = prv[j] + cost(j, mid); opt = j; }
        cur[mid] = best;
        solve(l, mid - 1, optl, opt); solve(mid + 1, r, opt, optr);
    };
    rep(k, 0, K) { fill(all(cur), INF); solve(1, n, 0, n - 1); swap(prv, cur); }
    return prv[n];
}
```

## Knuth optimisation (interval DP)

```cpp
// sketch: dp[i][j] over closed intervals, C(i, j) Monge and monotone on inclusion
for (int i = 0; i < n; ++i) { dp[i][i] = base(i); opt[i][i] = i; }
for (int len = 2; len <= n; ++len)
    for (int i = 0; i + len - 1 < n; ++i) {
        int j = i + len - 1; dp[i][j] = INF;
        for (int k = opt[i][j - 1]; k <= min(j - 1, opt[i + 1][j]); ++k) {
            ll v = dp[i][k] + dp[k + 1][j] + C(i, j);
            if (v < dp[i][j]) { dp[i][j] = v; opt[i][j] = k; }
        }
    }
```

## Aliens trick (Lagrangian relaxation)

If `f(k)` = optimal cost with exactly k groups/items is **convex** in k, then for a penalty λ solve the unconstrained problem where every group costs an extra λ: `g(λ) = min_k f(k) + λk`. Track the number of groups used (minimum count among ties). Binary search λ (integer) so that the count is ≤ k with smallest λ; answer `f(k) = g(λ) − λ·k`. Turns O(n·k) states into O(n) per λ.

## Slope trick

Maintain a convex piecewise-linear function by the multiset of its slope-change points. Classic: minimum Σ|a_i − b_i| to make b non-decreasing:

```cpp
ll minCostNonDecreasing(const vector<ll>& a) {
    priority_queue<ll> pq; ll ans = 0;
    for (ll x : a) {
        pq.push(x);
        if (pq.top() > x) { ans += pq.top() - x; pq.pop(); pq.push(x); }
    }
    return ans;   // strictly increasing: apply to a_i - i
}
```
