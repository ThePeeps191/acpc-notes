# SCC, 2-SAT, Bridges, Articulation Points, Euler Paths

## Strongly connected components (Tarjan)

```cpp
// [stress-tested] comp[v] in [0, ncomp). Components are numbered in REVERSE topological order
// (comp 0 is a sink): edge u->v between components implies comp[u] >= comp[v].
struct SCC {
    int n, timer = 0, ncomp = 0;
    vector<vector<int>> g; vector<int> tin, low, comp, st; vector<char> on;
    SCC(const vector<vector<int>>& g) : n(sz(g)), g(g), tin(n, -1), low(n), comp(n, -1), on(n) {
        rep(v, 0, n) if (tin[v] == -1) dfs(v);
    }
    void dfs(int v) {
        tin[v] = low[v] = timer++; st.push_back(v); on[v] = 1;
        for (int u : g[v]) {
            if (tin[u] == -1) { dfs(u); low[v] = min(low[v], low[u]); }
            else if (on[u]) low[v] = min(low[v], tin[u]);
        }
        if (low[v] == tin[v]) {
            while (true) {
                int u = st.back(); st.pop_back(); on[u] = 0; comp[u] = ncomp;
                if (u == v) break;
            }
            ++ncomp;
        }
    }
};
```

Condensation DAG: edge comp[u] → comp[v] for every edge with different comps. Typical follow-ups: min edges to add to make strongly connected = max(#sources, #sinks) (0 if one SCC); nodes reachable from all others = the unique sink SCC if exactly one sink.

Recursion depth = n; for n ≥ 5·10^5 on a path consider an iterative version or Kosaraju with explicit stacks.

## 2-SAT

```cpp
// [stress-tested] variables 0..n-1. lit(x, true) = x, lit(x, false) = not x.
struct TwoSat {
    int n; vector<vector<int>> g;
    TwoSat(int n) : n(n), g(2 * n) {}
    static int lit(int x, bool val) { return 2 * x + !val; }
    void implies(int a, int b) { g[a].push_back(b); g[b ^ 1].push_back(a ^ 1); }  // a -> b (and !b -> !a)
    void either(int a, int b) { implies(a ^ 1, b); }                              // a or b
    void setTrue(int a) { implies(a ^ 1, a); }
    void atMostOne(int a, int b) { implies(a, b ^ 1); }                           // not (a and b)
    void equal(int a, int b) { implies(a, b); implies(b, a); }
    bool solve(vector<bool>& val) {
        SCC s(g); val.assign(n, false);
        rep(x, 0, n) {
            if (s.comp[2 * x] == s.comp[2 * x + 1]) return false;
            val[x] = s.comp[2 * x] < s.comp[2 * x + 1];   // later in topo order = true
        }
        return true;
    }
};
```

Modelling: each boolean choice is a variable; constraints must be clauses of **two** literals. "Exactly one of a, b" = either(a, b) + atMostOne(a, b). "At most one among k literals" → prefix-variable encoding (O(k) extra variables). Often combined with binary search on a threshold (e.g. maximise min distance between chosen points: pairs closer than D can't both be chosen).

## Bridges and articulation points

```cpp
// [stress-tested] undirected multigraph; edges given as (u, v). Handles parallel edges via edge ids.
struct BridgeCut {
    int n, timer = 0; vector<vector<pii>> g; vector<int> tin, low;
    vector<char> isBridge, isCut;
    BridgeCut(int n, const vector<pii>& edges)
        : n(n), g(n), tin(n, -1), low(n), isBridge(sz(edges)), isCut(n) {
        rep(i, 0, sz(edges)) { auto [u, v] = edges[i]; g[u].push_back({v, i}); g[v].push_back({u, i}); }
        rep(v, 0, n) if (tin[v] == -1) dfs(v, -1);
    }
    void dfs(int v, int pe) {
        tin[v] = low[v] = timer++; int children = 0;
        for (auto [u, id] : g[v]) {
            if (id == pe) continue;
            if (tin[u] != -1) { low[v] = min(low[v], tin[u]); continue; }
            dfs(u, id); low[v] = min(low[v], low[u]); ++children;
            if (low[u] > tin[v]) isBridge[id] = 1;
            if (low[u] >= tin[v] && pe != -1) isCut[v] = 1;
        }
        if (pe == -1 && children > 1) isCut[v] = 1;
    }
};
```

- **2-edge-connected components:** delete bridges, take components. Contracting them gives the **bridge tree** (a tree!) — then path questions become tree questions ("must every u–v path use edge e" ⇔ e is a bridge on the bridge-tree path).
- **Biconnected components / block-cut tree:** push edges on a stack in the DFS; when `low[u] >= tin[v]` pop edges up to (v, u) → one block. Block-cut tree: bipartite tree of blocks and cut vertices; "vertices that lie on every u–v path" = cut vertices on the block-cut tree path.
- Edge in a cycle ⇔ not a bridge. Graph stays connected after removing any one vertex ⇔ no articulation points.

## Euler path / circuit (Hierholzer)

Existence: **undirected** connected (ignoring isolated vertices) with 0 odd-degree vertices (circuit) or exactly 2 (path, start at an odd one). **Directed**: every vertex in = out (circuit), or exactly one with out − in = 1 (start) and one with in − out = 1 (end).

```cpp
// [stress-tested] returns vertex sequence of length m+1 using every edge once, or {} if impossible
// from s. directed=false: each edge usable in both directions.
vector<int> eulerPath(int n, const vector<pii>& edges, int s, bool directed) {
    int m = sz(edges); vector<vector<pii>> g(n);
    rep(i, 0, m) { auto [u, v] = edges[i]; g[u].push_back({v, i}); if (!directed) g[v].push_back({u, i}); }
    vector<int> ptr(n, 0), path, st = {s}; vector<char> used(m, 0);
    while (!st.empty()) {
        int v = st.back();
        while (ptr[v] < sz(g[v]) && used[g[v][ptr[v]].second]) ++ptr[v];
        if (ptr[v] == sz(g[v])) { path.push_back(v); st.pop_back(); }
        else { auto [u, id] = g[v][ptr[v]]; used[id] = 1; st.push_back(u); }
    }
    if (sz(path) != m + 1) return {};
    reverse(all(path));
    return path;
}
```

**Check the degree conditions yourself first** (and pick s accordingly): if they fail, the routine can still return a sequence of length m+1 that is not a valid trail. The length check only catches edges unreachable from s. Uses: **De Bruijn sequence** (nodes = (k−1)-length strings, edges = k-length strings, Euler circuit), reconstructing a string from all its k-mers, "draw without lifting the pen", domino chains.
