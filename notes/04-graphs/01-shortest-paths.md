# Shortest Paths and Graph Search

| situation | algorithm | complexity |
|---|---|---|
| unweighted | BFS | O(n + m) |
| weights 0/1 (or 0/k) | 0-1 BFS with deque | O(n + m) |
| non-negative weights | Dijkstra | O(m log n) |
| negative weights, detect negative cycle | Bellman–Ford | O(nm) |
| all pairs, n ≤ 500 | Floyd–Warshall | O(n³) |
| DAG (any weights) | DP in topological order | O(n + m) |

## BFS on grids (template)

```cpp
const int DR[] = {-1, 1, 0, 0}, DC[] = {0, 0, -1, 1};   // add diagonals for 8-neighbour
vector<vector<int>> gridBFS(const vector<string>& g, vector<pii> sources) {
    int R = sz(g), C = sz(g[0]);
    vector<vector<int>> d(R, vector<int>(C, -1));
    queue<pii> q;
    for (auto [r, c] : sources) { d[r][c] = 0; q.push({r, c}); }   // multi-source
    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();
        rep(k, 0, 4) {
            int nr = r + DR[k], nc = c + DC[k];
            if (nr < 0 || nr >= R || nc < 0 || nc >= C || g[nr][nc] == '#' || d[nr][nc] != -1) continue;
            d[nr][nc] = d[r][c] + 1; q.push({nr, nc});
        }
    }
    return d;
}
```

- **State-space BFS:** node = (cell, extra state): direction, die orientation (ACPC 2016 *Dacey the Dice*), collected keys bitmask, remaining fuel. Encode as one integer `((r * C + c) * S + s)`.
- **Sliding moves** (ice: move until hitting an obstacle, ACPC 2024 *Slippery Floor*): neighbours of a cell are the 4 stopping cells; precompute or walk (each walk is O(R + C)).
- **Flood fill / count components**: BFS from every unvisited cell.

## 0-1 BFS

```cpp
// sketch: adj[u] = {(v, w)} with w in {0, 1}
deque<int> dq; dist.assign(n, INF); dist[s] = 0; dq.push_back(s);
while (!dq.empty()) {
    int u = dq.front(); dq.pop_front();
    for (auto [v, w] : adj[u]) if (dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        w ? dq.push_back(v) : dq.push_front(v);
    }
}
```

Typical: "minimum number of walls to break", "minimum number of direction changes" (state = (cell, direction), continuing straight costs 0, turning costs 1).

## Dijkstra

```cpp
// adj[u] = {(v, w)}, w >= 0. par gives the shortest path tree.
vector<ll> dijkstra(const vector<vector<pair<int, ll>>>& adj, const vector<int>& srcs, vector<int>* par = nullptr) {
    int n = sz(adj); vector<ll> d(n, INF);
    if (par) par->assign(n, -1);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> pq;
    for (int s : srcs) { d[s] = 0; pq.push({0, s}); }
    while (!pq.empty()) {
        auto [du, u] = pq.top(); pq.pop();
        if (du != d[u]) continue;                       // stale entry
        for (auto [v, w] : adj[u]) if (du + w < d[v]) {
            d[v] = du + w; pq.push({d[v], v});
            if (par) (*par)[v] = u;
        }
    }
    return d;
}
```

- Dense graph (n ≤ 5000, m ≈ n²): O(n²) Dijkstra with a linear scan for the minimum is faster.
- **Count shortest paths:** when `du + w == d[v]` add `cnt[u]` to `cnt[v]` (process in pop order).
- **Edge (u,v,w) lies on some shortest s–t path** iff `ds[u] + w + dt[v] == ds[t]` (run from s and from t on the reversed graph).
- **Product graph Dijkstra** (ACPC 2023 *Horse Snatchers*: state = (town, still has horse)); (node, number of coupons used ≤ k); (node, time mod k). Size n·S nodes.
- **Maximise the minimum along the path** (bottleneck/widest path): Dijkstra with `max-heap` on `min(cap so far, w)`, or Kruskal in decreasing order until s, t connected, or binary search + BFS.
- **Binary search + Dijkstra** (ACPC 2018 *Arachnophobia*): multi-source Dijkstra from all spiders gives `spiderDist[v]`; binary search D, run Dijkstra s→t using only vertices with `spiderDist ≥ D`, check `dist ≤ T`.
- Vertex weights: put the weight on incoming edges. Undirected: add both directions.
- Shortest path with "time-dependent" edges that are FIFO (arriving later never lets you leave earlier): Dijkstra still works with arrival-time relaxation.

### Dijkstra on residues: which totals are representable (coin problem)

Coins c_1..c_k (unbounded), smallest coin `a`. `d[r]` = smallest representable total ≡ r (mod a). Then X is representable ⇔ `d[X mod a] ≤ X`. Largest non-representable = max(d[r]) − a (if gcd = 1). ACPC 2022 *Braaaains*.

```cpp
vector<ll> residueDist(const vector<ll>& coins) {
    ll a = *min_element(all(coins));
    vector<ll> d(a, INF); d[0] = 0;
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> pq; pq.push({0, 0});
    while (!pq.empty()) {
        auto [du, r] = pq.top(); pq.pop();
        if (du != d[r]) continue;
        for (ll c : coins) { int nr = (r + c) % a; if (du + c < d[nr]) { d[nr] = du + c; pq.push({d[nr], nr}); } }
    }
    return d;   // X representable iff d[X % a] <= X
}
```

## Bellman–Ford and negative cycles

```cpp
// edges (u, v, w). Returns dist; dist = -INF for vertices reachable from a negative cycle.
vector<ll> bellmanFord(int n, const vector<array<ll, 3>>& edges, int s) {
    vector<ll> d(n, INF); d[s] = 0;
    rep(it, 0, n) for (auto [u, v, w] : edges)
        if (d[u] < INF && d[u] + w < d[v]) d[v] = (it == n - 1 ? -INF : d[u] + w);
    rep(it, 0, n) for (auto [u, v, w] : edges)          // propagate -INF
        if (d[u] == -INF && d[v] != -INF) d[v] = -INF;
    return d;
}
```

Marking only in the n-th round misses some affected vertices, so the second loop propagates `-INF` through everything reachable. **Difference constraints** `x_v − x_u ≤ w` → edge u → v with weight w; add a source with 0-edges to all; feasible iff no negative cycle; `x = dist` is a solution (maximal for each variable with x ≤ 0).

## Floyd–Warshall

```cpp
// d[i][j] = edge weight or INF, d[i][i] = 0. Negative cycle through i iff d[i][i] < 0 afterwards.
void floyd(vector<vector<ll>>& d) {
    int n = sz(d);
    rep(k, 0, n) rep(i, 0, n) if (d[i][k] < INF) rep(j, 0, n)
        if (d[k][j] < INF) d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
}
```

Variants (same triple loop): transitive closure (`reach[i][j] |= reach[i][k] && reach[k][j]`, or bitset rows), minimax path (`max` instead of `+`, `min` outer), counting paths in a DAG, shortest path when vertices are "unlocked" over time (process k in order of unlock time and answer queries in between).
