# Trees: LCA, Euler Tour, HLD, Centroid Decomposition

## LCA with binary lifting (+ distances, k-th ancestor, path aggregates)

```cpp
// [stress-tested] weighted tree, iterative DFS (safe for deep trees).
struct LCA {
    int n, LOG; vector<vector<int>> up; vector<int> dep, tin, tout; vector<ll> dist;
    LCA(const vector<vector<pair<int, ll>>>& g, int root = 0) : n(sz(g)) {
        LOG = 1; while ((1 << LOG) < n) ++LOG;
        up.assign(LOG, vector<int>(n, root)); dep.assign(n, 0); tin.assign(n, 0); tout.assign(n, 0);
        dist.assign(n, 0);
        int timer = 0; vector<pair<int, int>> st = {{root, 0}};   // (vertex, next child index)
        tin[root] = timer++;
        while (!st.empty()) {
            auto& [v, i] = st.back();
            if (i < sz(g[v])) {
                auto [u, w] = g[v][i++];
                if (u == up[0][v] && v != root) continue;
                up[0][u] = v; dep[u] = dep[v] + 1; dist[u] = dist[v] + w; tin[u] = timer++;
                st.push_back({u, 0});
            } else { tout[v] = timer++; st.pop_back(); }
        }
        rep(k, 1, LOG) rep(v, 0, n) up[k][v] = up[k - 1][up[k - 1][v]];
    }
    bool isAnc(int u, int v) const { return tin[u] <= tin[v] && tout[v] <= tout[u]; }
    int lca(int u, int v) const {
        if (isAnc(u, v)) return u;
        if (isAnc(v, u)) return v;
        for (int k = LOG - 1; k >= 0; --k) if (!isAnc(up[k][u], v)) u = up[k][u];
        return up[0][u];
    }
    ll distance(int u, int v) const { return dist[u] + dist[v] - 2 * dist[lca(u, v)]; }
    int kthAncestor(int v, int k) const {   // k <= dep[v]
        rep(j, 0, LOG) if (k >> j & 1) v = up[j][v];
        return v;
    }
    // k-th vertex on the path u -> v (0-indexed), assumes k <= #edges on the path
    int kthOnPath(int u, int v, int k) const {
        int w = lca(u, v), du = dep[u] - dep[w], dv = dep[v] - dep[w];
        return k <= du ? kthAncestor(u, k) : kthAncestor(v, du + dv - k);
    }
};
```

- **Path aggregates (max edge on path, min, sum):** add a table `mx[k][v]` = max edge weight on the 2^k jump; combine while lifting both endpoints to the LCA.
- **Path sums with point updates:** Euler tour + BIT: value on vertex v contributes to the whole subtree, so add at `tin[v]`, subtract at `tout[v]`; root-to-v sum = prefix query at `tin[v]`; path(u, v) = s(u) + s(v) − 2 s(lca) + val(lca).
- **Subtree queries:** with pre-order indices `tin` (counting only entries), subtree of v = `[tin[v], tin[v] + size[v] − 1]` → BIT/segment tree.
- **Distance between nodes in unweighted tree, many queries:** depth[u] + depth[v] − 2·depth[lca].
- **Ancestor check** without LCA: tin/tout as above (O(1)).
- **Diameter endpoints:** farthest from any node is an endpoint a; farthest from a is b. Every vertex's eccentricity = max(dist(v, a), dist(v, b)). **Centre** = middle of the diameter path.
- **Virtual (auxiliary) tree** for queries on k marked nodes: sort by tin, add LCAs of adjacent pairs, build with a stack → O(k log n) nodes; then run the DP only on that tree.

## Heavy-light decomposition

Any root-to-leaf path crosses O(log n) heavy chains; positions on a chain are contiguous, so path queries become O(log n) segment-tree range queries (O(log² n) total).

```cpp
// [stress-tested] call path(u, v, f): f(l, r) on inclusive position ranges. Use with any segment tree
// indexed by pos[]. Edge values: store the edge weight at pos[child] and pass skipLca = true.
struct HLD {
    int n; vector<int> par, heavy, dep, head, pos, sub;
    HLD(const vector<vector<int>>& g, int root = 0) : n(sz(g)), par(n, -1), heavy(n, -1), dep(n), head(n), pos(n), sub(n, 1) {
        vector<int> order; order.reserve(n); order.push_back(root);
        rep(i, 0, n) { int v = order[i]; for (int u : g[v]) if (u != par[v]) { par[u] = v; dep[u] = dep[v] + 1; order.push_back(u); } }
        for (int i = n - 1; i > 0; --i) { int v = order[i]; sub[par[v]] += sub[v]; }
        rep(i, 0, n) { int v = order[i]; int best = 0;
            for (int u : g[v]) if (u != par[v] && sub[u] > best) { best = sub[u]; heavy[v] = u; } }
        int cur = 0; vector<int> st = {root}; head[root] = root;
        while (!st.empty()) {                       // pre-order DFS, heavy child visited first
            int v = st.back(); st.pop_back(); pos[v] = cur++;
            for (int u : g[v]) if (u != par[v] && u != heavy[v]) { head[u] = u; st.push_back(u); }
            if (heavy[v] != -1) { head[heavy[v]] = head[v]; st.push_back(heavy[v]); }
        }
    }
    template <class F> void path(int u, int v, F f, bool skipLca = false) {
        for (; head[u] != head[v]; v = par[head[v]]) {
            if (dep[head[u]] > dep[head[v]]) swap(u, v);
            f(pos[head[v]], pos[v]);
        }
        if (dep[u] > dep[v]) swap(u, v);
        if (pos[u] + skipLca <= pos[v]) f(pos[u] + skipLca, pos[v]);
    }
    pair<int, int> subtree(int v) { return {pos[v], pos[v] + sub[v] - 1}; }
};
```

Positions are a DFS pre-order with the heavy child first, so both chains and subtrees are contiguous: `subtree(v)` and `path(u, v)` can be mixed on one segment tree.

## Centroid decomposition

Centroid: vertex whose removal leaves components of size ≤ n/2. Recursively decompose: depth O(log n); every path in the tree passes through the centroid of the smallest decomposition component containing both ends.

```cpp
// cpar[v] = parent in the centroid tree (-1 for the root centroid)
struct Centroid {
    vector<vector<int>> g; vector<int> sub, cpar; vector<char> removed;
    Centroid(const vector<vector<int>>& g) : g(g), sub(sz(g)), cpar(sz(g), -1), removed(sz(g)) { build(0, -1); }
    int calcSize(int v, int p) { sub[v] = 1; for (int u : g[v]) if (u != p && !removed[u]) sub[v] += calcSize(u, v); return sub[v]; }
    int find(int v, int p, int tot) {
        for (int u : g[v]) if (u != p && !removed[u] && sub[u] * 2 > tot) return find(u, v, tot);
        return v;
    }
    void build(int v, int p) {
        int c = find(v, -1, calcSize(v, -1));
        cpar[c] = p; removed[c] = 1;
        // ---- process all paths through c here (DFS from c over non-removed vertices) ----
        for (int u : g[c]) if (!removed[u]) build(u, c);
    }
};
```

Typical uses:
- **Count paths with length ≤ K / exactly K:** at each centroid, collect depths from each child subtree; count pairs across different subtrees (sort + two pointers, subtract pairs within the same child).
- **Nearest marked vertex queries:** `best[c]` = min distance from centroid c to a marked vertex in its component; update/query walks up `cpar` (O(log n) ancestors) using `dist(v, c)` (precomputed per level or via LCA).
- Recursion depth of `calcSize` can be n on a path graph at the top level; fine for n ≤ 2·10^5 on Kattis, otherwise make it iterative.
