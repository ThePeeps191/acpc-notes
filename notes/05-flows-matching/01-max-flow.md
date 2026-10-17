# Max Flow, Min Cut, and Modelling

Signals: "each X used at most once/at most c times", "assign", "disjoint paths", "minimum set of things to remove to disconnect", "choose a subset with dependencies maximising profit", "minimum number of paths/chains covering …". Small n (≤ 500 nodes, ≤ 10^4–10^5 edges) is a strong hint.

## Dinic

```cpp
// [stress-tested] O(V^2 E) worst, much faster in practice; unit-capacity graphs O(E sqrt V).
struct Dinic {
    struct E { int to; ll cap; };
    vector<E> e; vector<vector<int>> g; vector<int> level, it;
    Dinic(int n) : g(n), level(n), it(n) {}
    int addEdge(int u, int v, ll cap, ll rcap = 0) {        // rcap = cap for undirected edges
        g[u].push_back(sz(e)); e.push_back({v, cap});
        g[v].push_back(sz(e)); e.push_back({u, rcap});
        return sz(e) - 2;                                    // id; flow on it = original cap - e[id].cap
    }
    bool bfs(int s, int t) {
        fill(all(level), -1); level[s] = 0; queue<int> q; q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (int id : g[v]) if (e[id].cap > 0 && level[e[id].to] < 0) {
                level[e[id].to] = level[v] + 1; q.push(e[id].to);
            }
        }
        return level[t] >= 0;
    }
    ll dfs(int v, int t, ll f) {
        if (v == t) return f;
        ll pushed = 0;
        for (int& i = it[v]; i < sz(g[v]); ++i) {
            int id = g[v][i], to = e[id].to;
            if (e[id].cap <= 0 || level[to] != level[v] + 1) continue;
            ll d = dfs(to, t, min(f - pushed, e[id].cap));
            if (d > 0) {
                e[id].cap -= d; e[id ^ 1].cap += d; pushed += d;
                if (pushed == f) return pushed;
            }
        }
        return pushed;
    }
    ll maxflow(int s, int t, ll limit = INF) {
        ll flow = 0;
        while (flow < limit && bfs(s, t)) {
            fill(all(it), 0);
            flow += dfs(s, t, limit - flow);
        }
        return flow;
    }
    // after maxflow: S side of a minimum cut = vertices reachable from s in the residual graph
    bool inS(int v) const { return level[v] >= 0; }
};
```

- **Min cut edges:** edges u → v with `inS(u) && !inS(v)` (original edges only). Max flow = min cut capacity.
- Reuse: to rerun with modified capacities, rebuild the graph (or reset caps from a saved copy).
- Capacities as `ll`; use `INF` for "uncuttable" edges but make sure the answer can't involve them (otherwise "impossible").

## Modelling cookbook

| need | construction |
|---|---|
| several sources/sinks | super source S → each source (cap = supply), each sink → super sink T |
| vertex capacity c | split v into v_in → v_out with cap c; edges enter v_in, leave v_out |
| undirected edge cap c | one edge with cap c in both directions (`addEdge(u, v, c, c)`) |
| k edge-disjoint s–t paths | unit capacities, flow ≥ k (Menger); vertex-disjoint: split vertices with cap 1 |
| bipartite matching | S → left (1), left → right (1), right → T (1) |
| each worker ≤ a_i tasks, task needs b_j workers | S → worker (a_i), worker → task (1), task → T (b_j); feasible iff flow = Σb_j |
| minimise the maximum load | binary search the cap L on S → worker edges; feasible iff max flow = total |
| time steps (people moving over time) | time-expanded graph: node (v, t) for every t, edges (v,t) → (u,t+1) |
| pick at most one per row & column (grid) | bipartite rows ↔ columns, cell = edge |
| choose ≥/≤ constraints from both sides | lower bounds (below) |

### Project selection / maximum closure (min cut)

Projects with profit p_i (may be negative), dependencies "taking i requires taking j". Build: S → i with cap p_i for p_i > 0; i → T with cap −p_i for p_i < 0; i → j with cap INF for each dependency. **Max profit = Σ positive p_i − maxflow.** Chosen set = S side.

General "two-label" min cut (each item either A or B): cost a_i if item is in B (edge S → i, cap a_i), cost b_i if in A (edge i → T, cap b_i), penalty c for i in A and j in B (edge i → j, cap c). Works when penalties are non-negative and pairwise ("submodular"). Image segmentation, "assign each student to one of two buildings with friendship penalties".

### Lower bounds (circulations with demands)

Edge u → v with flow in [l, c]: add edge with cap c − l, and record `ex[v] += l`, `ex[u] −= l`. Then S' → v (cap ex[v]) for ex[v] > 0, v → T' (cap −ex[v]) for ex[v] < 0. A feasible circulation exists iff maxflow(S', T') saturates all S' edges. For an s–t flow add edge t → s with cap INF first.

- **Max flow with lower bounds:** after feasibility, F = flow on the t → s edge; remove that edge (set both caps to 0); answer = F + maxflow(s, t) on the same residual graph (S', T' edges are saturated and can't be used).
- **Min flow with lower bounds:** feasible value F = flow on the t → s edge; remove that edge (set both caps to 0); answer = F − maxflow(t, s).

```cpp
// [stress-tested] Minimum number of s->t paths in a DAG covering every edge at least once
// (ACPC 2022 Escape Routes). Every edge: lower bound 1, capacity INF. Assumes each edge lies on
// some s->t path. Generalises: give each edge its own [l, c].
ll minFlowCoverEdges(int n, const vector<pii>& edges, int s, int t) {
    int S = n, T = n + 1; Dinic d(n + 2); vector<ll> ex(n, 0);
    const ll BIG = 1e12;
    for (auto [u, v] : edges) { d.addEdge(u, v, BIG - 1); ex[v] += 1; ex[u] -= 1; }
    int ts = d.addEdge(t, s, BIG);
    ll need = 0;
    rep(v, 0, n) {
        if (ex[v] > 0) { d.addEdge(S, v, ex[v]); need += ex[v]; }
        else if (ex[v] < 0) d.addEdge(v, T, -ex[v]);
    }
    if (d.maxflow(S, T) != need) return -1;               // infeasible
    ll F = BIG - d.e[ts].cap;                               // current s->t flow value
    d.e[ts].cap = d.e[ts ^ 1].cap = 0;
    return F - d.maxflow(t, s);
}
```

## Flow theorems worth knowing

- **Max-flow = min-cut.** Integral capacities ⇒ integral max flow (so flows decompose into paths: each unit = one path).
- **Menger:** max #edge-disjoint s–t paths = min #edges to delete to disconnect s from t (same for vertices with splitting).
- **Hall:** a bipartite graph has a matching saturating the left side iff every left subset X has |N(X)| ≥ |X|. Deficiency version: max matching = |L| − max_X (|X| − |N(X)|).
- Max flow in a graph where all capacities are multiplied by k is k times — so "fractional" answers often come from binary search on a ratio, not from fractional flows.
- **Gomory–Hu tree** (all-pairs min cut, n − 1 max flows) exists if you need min cut between many pairs in an undirected graph.
