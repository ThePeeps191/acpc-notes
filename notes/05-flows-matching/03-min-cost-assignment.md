# Min-Cost Flow and the Assignment Problem

## Hungarian algorithm (n ≤ m, O(n²m))

```cpp
// [stress-tested] a = n x m cost matrix (n <= m). Minimum total cost assigning every row to a
// distinct column. Returns {cost, match} with match[row] = column. Maximise: negate costs.
pair<ll, vector<int>> hungarian(const vector<vector<ll>>& a) {
    int n = sz(a), m = sz(a[0]);
    const ll BIG = LLONG_MAX / 4;
    vector<ll> u(n + 1), v(m + 1); vector<int> p(m + 1), way(m + 1);
    rep(i, 1, n + 1) {
        p[0] = i; int j0 = 0;
        vector<ll> minv(m + 1, BIG); vector<char> used(m + 1, 0);
        do {
            used[j0] = 1; int i0 = p[j0], j1 = 0; ll delta = BIG;
            rep(j, 1, m + 1) if (!used[j]) {
                ll cur = a[i0 - 1][j - 1] - u[i0] - v[j];
                if (cur < minv[j]) { minv[j] = cur; way[j] = j0; }
                if (minv[j] < delta) { delta = minv[j]; j1 = j; }
            }
            rep(j, 0, m + 1) if (used[j]) { u[p[j]] += delta; v[j] -= delta; } else minv[j] -= delta;
            j0 = j1;
        } while (p[j0] != 0);
        do { int j1 = way[j0]; p[j0] = p[j1]; j0 = j1; } while (j0);
    }
    vector<int> match(n);
    rep(j, 1, m + 1) if (p[j]) match[p[j] - 1] = j - 1;
    return {-v[0], match};
}
```

- Forbidden pairs: cost = large constant `C` (e.g. 1e12, but ≤ BIG/n); if the result ≥ C, no valid assignment exists.
- n > m: transpose. "Not everyone must be matched": add dummy columns with cost 0 (or the cost of staying unmatched).
- 500 × 500 runs in well under a second.

### Worked pattern: enumerate the maximum, then assign (ACPC 2018 *Dog Trouble*)

Minimise `Σ (t − t_i)` where t = max assigned time ⇒ `N·t − Σ t_i`. Enumerate t over all distinct values (≤ 200); allow only pairs with t_ij ≤ t; maximise Σ t_ij with Hungarian (cost −t_ij, forbidden = C). Answer = min over t of `N·t − maxSum(t)`. Using a t larger than the true max only overestimates, so taking the min over all t is exact. 200 × O(50³) is tiny.

General lesson: **an objective mixing "max" and "sum" → fix the max by enumeration or binary search, then optimise the sum.**

## Min-cost max-flow (successive shortest paths with SPFA)

```cpp
// [stress-tested] handles negative edge costs (no negative cycles). O(F · V · E) worst case.
struct MCMF {
    struct E { int to; ll cap, cost; };
    vector<E> e; vector<vector<int>> g; vector<ll> dist; vector<int> pe; vector<char> inq;
    MCMF(int n) : g(n), dist(n), pe(n), inq(n) {}
    int addEdge(int u, int v, ll cap, ll cost) {
        g[u].push_back(sz(e)); e.push_back({v, cap, cost});
        g[v].push_back(sz(e)); e.push_back({u, 0, -cost});
        return sz(e) - 2;
    }
    // returns {flow, cost}. stopIfPositive: stop when the cheapest path costs >= 0
    // (gives min-cost *any-amount* flow, i.e. max profit when costs are negated profits)
    pair<ll, ll> run(int s, int t, ll maxf = INF, bool stopIfPositive = false) {
        ll flow = 0, cost = 0;
        while (flow < maxf) {
            fill(all(dist), INF); dist[s] = 0; queue<int> q; q.push(s); inq[s] = 1;
            while (!q.empty()) {
                int u = q.front(); q.pop(); inq[u] = 0;
                for (int id : g[u]) {
                    const E& ed = e[id];
                    if (ed.cap > 0 && dist[u] + ed.cost < dist[ed.to]) {
                        dist[ed.to] = dist[u] + ed.cost; pe[ed.to] = id;
                        if (!inq[ed.to]) { inq[ed.to] = 1; q.push(ed.to); }
                    }
                }
            }
            if (dist[t] == INF || (stopIfPositive && dist[t] >= 0)) break;
            ll f = maxf - flow;
            for (int v = t; v != s; v = e[pe[v] ^ 1].to) f = min(f, e[pe[v]].cap);
            for (int v = t; v != s; v = e[pe[v] ^ 1].to) { e[pe[v]].cap -= f; e[pe[v] ^ 1].cap += f; }
            flow += f; cost += f * dist[t];
        }
        return {flow, cost};
    }
};
```

Modelling with costs:
- **Assignment with capacities** (worker i can do up to c_i jobs): S → worker (c_i, 0), worker → job (1, cost), job → T (1, 0).
- **k disjoint paths of minimum total length**: unit capacities, run with `maxf = k`.
- **Choose at most k items with conflicts along a line** (k non-overlapping interval families, max weight): chain i → i+1 with (k, 0), interval [l, r) as edge l → r with (1, −w).
- **Max-weight matching where not all must be matched:** costs = −weight, `stopIfPositive = true`.
- Convex costs (cost of the j-th unit is increasing): parallel edges with capacity 1 and costs c_1 ≤ c_2 ≤ ….
- If it's too slow (large flow), use Dijkstra with Johnson potentials instead of SPFA, or capacity scaling — rarely needed at ACPC sizes.
