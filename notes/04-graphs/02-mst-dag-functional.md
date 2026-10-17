# MST, DAGs, Functional Graphs, Permutations

## Minimum spanning tree

```cpp
// [stress-tested] Kruskal: edges (w, u, v). Returns total weight (n-1 edges if connected). Needs DSU (§3.1).
struct DSUk { vector<int> p; DSUk(int n) : p(n, -1) {}
    int f(int x) { return p[x] < 0 ? x : p[x] = f(p[x]); }
    bool u(int a, int b) {
        a = f(a); b = f(b); if (a == b) return 0;
        if (p[a] > p[b]) swap(a, b);
        p[a] += p[b]; p[b] = a; return 1;
    }
};
ll kruskal(int n, vector<tuple<ll, int, int>> edges, vector<tuple<ll, int, int>>* used = nullptr) {
    sort(all(edges)); DSUk d(n); ll tot = 0;
    for (auto& [w, a, b] : edges) if (d.u(a, b)) { tot += w; if (used) used->push_back({w, a, b}); }
    return tot;
}
```

- **Dense graphs** (complete graph on points, n ≤ 5000): Prim O(n²) with an array, no heap.
- **Cycle property:** the heaviest edge on any cycle is not needed. **Cut property:** the lightest edge across any cut is in some MST.
- MST minimises the **maximum** edge on paths too: the minimax path between u and v is the path in the MST. Max spanning tree for maximin paths.
- Second-best MST: for each non-tree edge (u, v, w), replace the max tree edge on path(u, v) (binary lifting with max, §5.4).
- **Min ratio spanning tree** Σcost/Σgain (ACPC 2023 *Buildin' Roads*): binary search λ, MST on `cost − λ·gain`, feasible iff total ≤ 0 (§2.1). Doubles, 60–100 iterations.
- Steiner tree on k ≤ 10 terminals: dp[mask][v] + Dijkstra per mask (O(3^k n + 2^k m log n)).
- Count spanning trees: Kirchhoff (determinant of Laplacian minor, §7.4).

## Topological sort and DAG DP

```cpp
// [stress-tested] Kahn. Returns empty vector if there is a cycle.
// Min-heap instead of queue => lexicographically smallest order.
vector<int> topo(const vector<vector<int>>& g) {
    int n = sz(g); vector<int> indeg(n), order;
    rep(u, 0, n) for (int v : g[u]) indeg[v]++;
    queue<int> q; rep(u, 0, n) if (!indeg[u]) q.push(u);
    while (!q.empty()) {
        int u = q.front(); q.pop(); order.push_back(u);
        for (int v : g[u]) if (--indeg[v] == 0) q.push(v);
    }
    return sz(order) == n ? order : vector<int>();
}
```

- Longest path / count paths / probability of reaching: DP in topo order. Number of paths can be huge → mod or doubles.
- Unique topological order ⇔ queue size is always 1 ⇔ Hamiltonian path exists in the DAG.
- **Min path cover** of a DAG (vertices) = n − max matching (§6.2); **with path overlap allowed** = same on the transitive closure. **Cover all edges** (ACPC 2022 *Escape Routes*): min flow with lower bounds (§6.1).
- **Dilworth:** minimum number of chains covering a poset = size of the largest antichain. For 2D dominance (pairs sorted by x, chains need y non-decreasing): min chains = longest strictly decreasing subsequence of y.
- Directed graph with cycles → condense SCCs first (§5.3), then DAG DP.

## Functional graphs (each node has exactly one outgoing edge)

Structure: each component is one cycle with in-trees hanging off it.

```cpp
// [stress-tested] cycId[v] = id of the cycle containing v (-1 if v is on a tail); cycLen[id] = its length
void functionalCycles(const vector<int>& f, vector<int>& cycId, vector<int>& cycLen) {
    int n = sz(f); vector<int> state(n, 0); cycId.assign(n, -1); cycLen.clear();
    rep(s, 0, n) if (!state[s]) {
        vector<int> path; int v = s;
        while (!state[v]) { state[v] = 1; path.push_back(v); v = f[v]; }   // 1 = on current path
        if (state[v] == 1) {                                                // found a new cycle
            int id = sz(cycLen), len = 0;
            for (int i = sz(path) - 1; ; --i) { cycId[path[i]] = id; ++len; if (path[i] == v) break; }
            cycLen.push_back(len);
        }
        for (int x : path) state[x] = 2;
    }
}
```

- **k-th successor** for huge k: binary lifting `up[j][v] = up[j−1][up[j−1][v]]`, O(n log k) build, O(log k) query. Or walk to the cycle and use k mod cycle length.
- Floyd's tortoise–hare finds a cycle in an implicit sequence x → f(x) with O(1) memory (e.g. pseudo-random generators, repeated states of a simulation: **detect the period, then skip ahead**).
- "Simulate 10^18 steps of a deterministic process" ⇒ the state repeats; store first-seen time of each state in a map, jump by the period.

## Permutations

- Decompose into cycles: `i → p[i]`. Minimum swaps (any two elements) to sort = n − #cycles. Adjacent swaps = #inversions (BIT, §3.2).
- p^k (apply k times): for each cycle of length L, shift by k mod L. Order of p = lcm of cycle lengths.
- Square root of a permutation, conjugacy: cycles of equal length pair up.
- Swaps restricted to graph edges (ACPC 2021 *Swapping Places*): within each connected component any permutation is achievable; construct by fixing leaves of a spanning tree first (route the right value to a leaf along the tree path, then delete the leaf) — O(n²) swaps.
- Rotations of a permutation maximising fixed points (ACPC 2022 *Maximum Fix*): element a_i is fixed by exactly one shift k = (i − a_i) mod n; count votes.
