# Bipartite Matching and Covers

## Hopcroft–Karp (O(E √V))

```cpp
// [stress-tested] left 0..n-1, right 0..m-1
struct HopcroftKarp {
    int n, m; vector<vector<int>> g; vector<int> mL, mR, dist;
    HopcroftKarp(int n, int m) : n(n), m(m), g(n), mL(n, -1), mR(m, -1), dist(n) {}
    void addEdge(int u, int v) { g[u].push_back(v); }
    bool bfs() {
        queue<int> q; bool found = false;
        rep(u, 0, n) { if (mL[u] < 0) { dist[u] = 0; q.push(u); } else dist[u] = -1; }
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : g[u]) {
                int w = mR[v];
                if (w < 0) found = true;
                else if (dist[w] < 0) { dist[w] = dist[u] + 1; q.push(w); }
            }
        }
        return found;
    }
    bool dfs(int u) {
        for (int v : g[u]) {
            int w = mR[v];
            if (w < 0 || (dist[w] == dist[u] + 1 && dfs(w))) { mL[u] = v; mR[v] = u; return true; }
        }
        dist[u] = -1;
        return false;
    }
    int maxMatching() {
        int res = 0;
        while (bfs()) rep(u, 0, n) if (mL[u] < 0 && dfs(u)) ++res;
        return res;
    }
    // König: minimum vertex cover (call after maxMatching). Returns (left cover, right cover).
    pair<vector<int>, vector<int>> minVertexCover() {
        vector<char> visL(n, 0), visR(m, 0); queue<int> q;
        rep(u, 0, n) if (mL[u] < 0) { visL[u] = 1; q.push(u); }
        while (!q.empty()) {                                   // alternating BFS from free left
            int u = q.front(); q.pop();
            for (int v : g[u]) if (!visR[v] && mL[u] != v) {
                visR[v] = 1;
                if (mR[v] >= 0 && !visL[mR[v]]) { visL[mR[v]] = 1; q.push(mR[v]); }
            }
        }
        vector<int> L, R;
        rep(u, 0, n) if (!visL[u]) L.push_back(u);
        rep(v, 0, m) if (visR[v]) R.push_back(v);
        return {L, R};
    }
};
```

For tiny graphs, Kuhn's algorithm (DFS augment from each left vertex, O(VE)) is shorter; Hopcroft–Karp is safe up to ~10^5 vertices / 10^6 edges.

## Facts (bipartite graphs, V = L ∪ R)

| quantity | equals |
|---|---|
| min vertex cover | max matching (König) |
| max independent set | V − max matching (complement of a min vertex cover) |
| min edge cover (no isolated vertices) | V − max matching |
| min path cover of a DAG (vertex-disjoint) | n − max matching in split graph (u_out → v_in for each edge) |
| min path cover of a DAG (paths may share vertices) | same on the transitive closure |
| min chains covering a poset | max antichain (Dilworth) = min path cover on the closure |
| perfect matching exists | Hall's condition / max matching = n |

- **Grid problems** (dominoes, placing non-attacking pieces, covering cells): colour cells like a chessboard → bipartite. Max non-attacking rooks on a board with holes: rows ↔ columns.
- **Max independent set in a bipartite conflict graph** = choose max items with no conflicting pair (e.g. points where conflicts only between "even" and "odd" sums).
- **Min vertex cover in a bipartite graph** = "minimum number of lines (rows/cols) to cover all marked cells".
- **Maximum weight** bipartite matching → Hungarian / min cost flow (§6.3).
- **General (non-bipartite) graphs:** max matching needs Edmonds' blossom (O(V³)); for n ≤ 20–25, bitmask DP over matched vertices; for trees, greedy from leaves.
- **Path cover reconstruction**: matched edge u_out → v_in means "v follows u in its path"; paths start at vertices whose v_in is unmatched.

## Stable marriage (Gale–Shapley)

```cpp
// prefM[i] = men's preference lists (women ids, best first), prefW[j][i] = rank of man i for woman j
vector<int> galeShapley(const vector<vector<int>>& prefM, const vector<vector<int>>& rankW) {
    int n = sz(prefM); vector<int> wife(n, -1), husband(n, -1), nxt(n, 0); queue<int> freeMen;
    rep(i, 0, n) freeMen.push(i);
    while (!freeMen.empty()) {
        int m = freeMen.front(); freeMen.pop();
        int w = prefM[m][nxt[m]++];
        if (husband[w] < 0) { husband[w] = m; wife[m] = w; }
        else if (rankW[w][m] < rankW[w][husband[w]]) {
            wife[husband[w]] = -1; freeMen.push(husband[w]); husband[w] = m; wife[m] = w;
        } else freeMen.push(m);
    }
    return wife;   // man-optimal stable matching
}
```
