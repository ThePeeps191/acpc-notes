# Tree DP and Rerooting

Root the tree (usually at 0), compute children before parents. For n up to 10^6 use an **iterative** order: BFS/DFS order array, then process it in reverse.

```cpp
// [stress-tested] parent + pre-order. Bottom-up: for (int i = n-1; i >= 0; --i) { v = order[i]; ... }
void rootTree(const vector<vector<int>>& g, int root, vector<int>& par, vector<int>& order) {
    int n = sz(g); par.assign(n, -1); order.clear(); order.reserve(n);
    vector<int> st = {root}; par[root] = root;
    while (!st.empty()) {
        int v = st.back(); st.pop_back(); order.push_back(v);
        for (int u : g[v]) if (u != par[v]) { par[u] = v; st.push_back(u); }
    }
    par[root] = -1;
}
```

## Standard subtree DPs

- **Max independent set** (weights w): `in[v] = w_v + Σ out[c]`, `out[v] = Σ max(in[c], out[c])`.
- **Min vertex cover** = n − max independent set (trees are bipartite); **max matching** greedy from leaves: match a leaf with its parent if both free.
- **Diameter**: for each v keep the two longest downward paths `d1 ≥ d2`; diameter = max(d1 + d2). (Or BFS twice for unweighted/positive weights.)
- **Subtree sizes / sums**, **counting pairs at distance k**: small-to-large or centroid (§5.4).

### Colouring with k colours minimising cost (ACPC 2022 *City Numbers* pattern)

`dp[v][c]` = min cost of subtree of v when v gets colour c = cost(c) + Σ_children min_{c' ≠ c} dp[child][c']. The inner min over c' ≠ c is O(1) with the child's **best and second best**:

```cpp
// [stress-tested] colours 1..k, cost of colour c = c. Min total cost (k >= 2 is always feasible)
ll minColouring(const vector<vector<int>>& g, int k) {
    int n = sz(g); vector<int> par, order; rootTree(g, 0, par, order);
    vector<vector<ll>> dp(n, vector<ll>(k + 1, 0));
    for (int i = n - 1; i >= 0; --i) {
        int v = order[i];
        rep(c, 1, k + 1) dp[v][c] += c;
        if (par[v] < 0) continue;
        ll b1 = INF, b2 = INF; int bc = -1;          // best, second best over colours of v
        rep(c, 1, k + 1) {
            if (dp[v][c] < b1) { b2 = b1; b1 = dp[v][c]; bc = c; }
            else if (dp[v][c] < b2) b2 = dp[v][c];
        }
        rep(c, 1, k + 1) dp[par[v]][c] += (c == bc ? b2 : b1);
    }
    return *min_element(dp[0].begin() + 1, dp[0].end());
}
```

(Fact: an optimal colouring never needs more than O(log n) colours, so `k = min(k, 20)` is safe; with k = 1 and n > 1 it's impossible.)

## Tree knapsack (merge children with size bounds) — O(n²) total

`dp[v][j]` = best value choosing j nodes in subtree of v. Merging child c into v: loop `j ≤ size(v so far)`, `t ≤ size(c)`. Bounding loops by current sizes makes the total O(n²) (each pair of nodes is "merged" once at their LCA). With a cap K on j: O(nK).

```cpp
// sketch of merge step: cur = dp of v so far (size sv), ch = dp of child (size sc)
vector<ll> nd(sv + sc + 1, -INF);
rep(a, 0, sv + 1) if (cur[a] > -INF) rep(b, 0, sc + 1) if (ch[b] > -INF)
    nd[a + b] = max(nd[a + b], cur[a] + ch[b]);
cur = nd; sv += sc;
```

## Rerooting (answer for every root)

Two passes: `down[v]` (subtree answer) bottom-up, then `up[v]` (answer of the part outside v's subtree) top-down; combine. Requires: the contribution of a child to its parent can be computed from the child's value, and the combination is "removable" (sum) or handled with prefix/suffix aggregates (max).

```cpp
// [stress-tested] sum of distances from every vertex to all others, O(n)
vector<ll> sumDistAll(const vector<vector<int>>& g) {
    int n = sz(g); vector<int> par, order; rootTree(g, 0, par, order);
    vector<ll> sub(n, 1), down(n, 0), ans(n, 0);
    for (int i = n - 1; i > 0; --i) {
        int v = order[i]; sub[par[v]] += sub[v]; down[par[v]] += down[v] + sub[v];
    }
    ans[0] = down[0];
    rep(i, 1, n) { int v = order[i]; ans[v] = ans[par[v]] - sub[v] + (n - sub[v]); }
    return ans;
}
```

**Generic rerooting with max** (e.g. farthest node from every vertex): for each node compute children contributions `f(child)`, prefix-max and suffix-max over the children list, so the value "excluding child c" is `max(prefix[i−1], suffix[i+1], up-contribution of v)`.

## Other tree techniques (pointers)

- Euler tour (tin/tout) turns subtree into a contiguous range → BIT/segment tree on subtree queries (§5.4).
- "Is u an ancestor of v": `tin[u] ≤ tin[v] && tout[v] ≤ tout[u]` (ACPC 2020 *Listen To Your Boss*).
- DP on paths/LCA: binary lifting tables store aggregates (min/max edge) along 2^k jumps.
- Tree isomorphism: canonical hashing of sorted child hashes (AHU) from the centre(s).
