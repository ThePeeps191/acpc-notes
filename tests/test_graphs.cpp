// uses: 04-graphs/01-shortest-paths.md dijkstra residueDist bellmanFord floyd gridBFS
// uses: 04-graphs/02-mst-dag-functional.md kruskal topo functionalCycles
// uses: 04-graphs/03-connectivity-euler.md SCC TwoSat BridgeCut eulerPath
// uses: 04-graphs/04-trees.md LCA HLD Centroid
mt19937 rg(2024);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
int comps(int n, const vector<pii>& e, int skipE, int skipV) {
    vector<int> p(n); iota(all(p), 0);
    function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
    rep(i, 0, sz(e)) if (i != skipE && e[i].first != skipV && e[i].second != skipV) p[f(e[i].first)] = f(e[i].second);
    int c = 0; rep(i, 0, n) if (i != skipV && f(i) == i) c++;
    return c;
}
int main() {
    for (int it = 0; it < 1500; ++it) {
        int n = R(1, 8), m = R(0, 14);
        // ---- shortest paths
        vector<vector<pair<int, ll>>> adj(n); vector<array<ll, 3>> E; vector<vector<ll>> d(n, vector<ll>(n, INF));
        rep(i, 0, n) d[i][i] = 0;
        bool neg = R(0, 1);
        rep(i, 0, m) { int u = R(0, n - 1), v = R(0, n - 1); ll w = neg ? R(-3, 9) : R(0, 9);
            adj[u].push_back({v, w}); E.push_back({u, v, w}); d[u][v] = min(d[u][v], w); }
        auto fl = d; floyd(fl);
        int s = R(0, n - 1);
        auto bf = bellmanFord(n, E, s);
        rep(v, 0, n) {
            bool negReach = false;
            rep(k, 0, n) if (fl[k][k] < 0 && fl[s][k] < INF && fl[k][v] < INF) negReach = true;
            if (negReach) { CHECK(bf[v] == -INF); } else { CHECK(bf[v] == fl[s][v]); }
        }
        if (!neg) { auto dj = dijkstra(adj, {s}); rep(v, 0, n) CHECK(dj[v] == fl[s][v]); }
        // ---- residue dijkstra
        vector<ll> coins; rep(i, 0, R(1, 3)) coins.push_back(R(1, 9));
        auto rd = residueDist(coins); ll a0 = *min_element(all(coins));
        vector<char> rep_(200, 0); rep_[0] = 1;
        rep(x, 1, 200) for (ll c : coins) if (x >= c && rep_[x - c]) rep_[x] = 1;
        rep(x, 0, 200) CHECK((rd[x % a0] <= x) == (bool)rep_[x]);
        // ---- undirected stuff
        vector<pii> ue; rep(i, 0, m) ue.push_back({R(0, n - 1), R(0, n - 1)});
        ue.erase(remove_if(all(ue), [](pii p) { return p.first == p.second; }), ue.end());
        BridgeCut bc(n, ue);
        int base = comps(n, ue, -1, -1);
        rep(i, 0, sz(ue)) CHECK((bool)bc.isBridge[i] == (comps(n, ue, i, -1) > base));
        rep(v, 0, n) { int without = comps(n, ue, -1, v); bool isolated = true;
            for (auto [x, y] : ue) if (x == v || y == v) isolated = false;
            CHECK((bool)bc.isCut[v] == (!isolated && without > base)); }
        // kruskal vs brute over subsets (small)
        if (sz(ue) <= 10) {
            vector<tuple<ll, int, int>> we; for (auto [x, y] : ue) we.push_back({R(1, 9), x, y});
            ll best = INF;
            rep(mask, 0, 1 << sz(we)) { if (__builtin_popcount(mask) != n - base) continue;
                vector<pii> sel; ll w = 0; rep(i, 0, sz(we)) if (mask >> i & 1) { sel.push_back({get<1>(we[i]), get<2>(we[i])}); w += get<0>(we[i]); }
                if (comps(n, sel, -1, -1) == base) best = min(best, w); }
            CHECK(kruskal(n, we) == best);
        }
        // euler (undirected + directed)
        {
            vector<int> deg(n); for (auto [x, y] : ue) deg[x]++, deg[y]++;
            int odd = 0, st = ue.empty() ? 0 : ue[0].first; rep(v, 0, n) if (deg[v] & 1) odd++, st = v;
            auto p = eulerPath(n, ue, st, false);
            bool condU = (odd == 0 || odd == 2);
            if (!condU) p.clear();
            if (!p.empty()) { CHECK(sz(p) == sz(ue) + 1); multiset<pii> left; for (auto [x, y] : ue) left.insert({min(x, y), max(x, y)});
                rep(i, 0, sz(ue)) { auto itx = left.find({min(p[i], p[i + 1]), max(p[i], p[i + 1])}); CHECK(itx != left.end()); left.erase(itx); } }
            bool conn = true; { vector<pii> e2 = ue; int c = comps(n, e2, -1, -1); int iso = 0; rep(v, 0, n) if (!deg[v]) iso++; conn = (c - iso <= 1); }
            CHECK(p.empty() == !(conn && (odd == 0 || odd == 2)));
            vector<int> in(n), out(n); for (auto [x, y] : ue) out[x]++, in[y]++;
            int sDir = ue.empty() ? 0 : ue[0].first, bad = 0, plus = 0, minus = 0;
            rep(v, 0, n) { int dlt = out[v] - in[v]; if (dlt == 1) plus++, sDir = v; else if (dlt == -1) minus++; else if (dlt) bad++; }
            if (!bad && plus == minus && plus <= 1) {
                auto q = eulerPath(n, ue, sDir, true);
                if (!q.empty()) { multiset<pii> left(all(ue)); rep(i, 0, sz(ue)) { auto itx = left.find({q[i], q[i + 1]}); CHECK(itx != left.end()); left.erase(itx); } }
                else CHECK(!conn);
            }
        }
        // ---- directed: SCC + topo
        vector<vector<int>> g(n); vector<vector<int>> reach(n, vector<int>(n, 0));
        rep(i, 0, n) reach[i][i] = 1;
        rep(i, 0, m) { int u = R(0, n - 1), v = R(0, n - 1); g[u].push_back(v); reach[u][v] = 1; }
        rep(k, 0, n) rep(i, 0, n) rep(j, 0, n) if (reach[i][k] && reach[k][j]) reach[i][j] = 1;
        SCC sc(g);
        rep(u, 0, n) rep(v, 0, n) { CHECK((sc.comp[u] == sc.comp[v]) == (reach[u][v] && reach[v][u]));
            if (reach[u][v]) { CHECK(sc.comp[u] >= sc.comp[v]); } }
        auto tp = topo(g); bool cyc = false; rep(u, 0, n) for (int v : g[u]) if (reach[v][u]) cyc = true;
        CHECK(tp.empty() == cyc);
        if (!cyc) { vector<int> where(n); rep(i, 0, n) where[tp[i]] = i; rep(u, 0, n) for (int v : g[u]) CHECK(where[u] < where[v]); }
        // functional graph
        vector<int> f(n); for (auto& x : f) x = R(0, n - 1);
        vector<int> cid, clen; functionalCycles(f, cid, clen);
        rep(v, 0, n) { int x = v; bool onCyc = false; rep(t, 0, n) { x = f[x]; if (x == v) { onCyc = true; break; } }
            CHECK(onCyc == (cid[v] != -1));
            if (onCyc) { int L = 0; x = v; do { x = f[x]; L++; } while (x != v); CHECK(clen[cid[v]] == L); } }
        // ---- 2-SAT vs brute
        int nv = R(1, 5); TwoSat ts(nv); vector<array<int, 2>> cl;
        rep(i, 0, R(0, 8)) { int a = TwoSat::lit(R(0, nv - 1), R(0, 1)), b = TwoSat::lit(R(0, nv - 1), R(0, 1)); ts.either(a, b); cl.push_back({a, b}); }
        bool sat = false; rep(mask, 0, 1 << nv) { bool ok = true; for (auto [a, b] : cl) { bool va = (mask >> (a / 2) & 1) ^ (a & 1), vb = (mask >> (b / 2) & 1) ^ (b & 1); if (!va && !vb) ok = false; } sat |= ok; }
        vector<bool> val; bool got = ts.solve(val); CHECK(got == sat);
        if (got) for (auto [a, b] : cl) { bool va = val[a / 2] ^ (a & 1), vb = val[b / 2] ^ (b & 1); CHECK(va || vb); }
        // ---- trees
        int tn = R(1, 12); vector<vector<pair<int, ll>>> tg(tn); vector<vector<int>> ug(tn); vector<int> par(tn, -1);
        rep(i, 1, tn) { int p = R(0, i - 1); ll w = R(1, 9); tg[i].push_back({p, w}); tg[p].push_back({i, w}); ug[i].push_back(p); ug[p].push_back(i); par[i] = p; }
        int root = R(0, tn - 1); LCA L(tg, root); HLD H(ug, root); Centroid C(ug);
        // brute: dist from every node by DFS, path vertex lists
        auto pathOf = [&](int u, int v) {
            vector<int> prev(tn, -2); queue<int> q; q.push(u); prev[u] = -1;
            while (!q.empty()) { int x = q.front(); q.pop(); for (auto [y, w] : tg[x]) if (prev[y] == -2) prev[y] = x, q.push(y); }
            vector<int> pth; for (int x = v; x != -1; x = prev[x]) pth.push_back(x); reverse(all(pth)); return pth; };
        vector<ll> val2(tn); for (auto& x : val2) x = R(-5, 5);
        rep(q, 0, 20) { int u = R(0, tn - 1), v = R(0, tn - 1); auto pth = pathOf(u, v);
            ll dd = 0; rep(i, 0, sz(pth) - 1) for (auto [y, w] : tg[pth[i]]) if (y == pth[i + 1]) dd += w;
            CHECK(L.distance(u, v) == dd);
            int lc = pth[0]; for (int x : pth) if (L.dep[x] < L.dep[lc]) lc = x; CHECK(L.lca(u, v) == lc);
            int k = R(0, sz(pth) - 1); CHECK(L.kthOnPath(u, v, k) == pth[k]);
            vector<ll> arr(tn); rep(x, 0, tn) arr[H.pos[x]] = val2[x];
            ll hs = 0; H.path(u, v, [&](int l, int r) { rep(i, l, r + 1) hs += arr[i]; });
            ll bs = 0; for (int x : pth) bs += val2[x]; CHECK(hs == bs);
            ll hs2 = 0; H.path(u, v, [&](int l, int r) { rep(i, l, r + 1) hs2 += arr[i]; }, true); CHECK(hs2 == bs - val2[lc]);
            auto [sl, sr] = H.subtree(u); set<int> subs; rep(x, 0, tn) if (L.isAnc(u, x)) subs.insert(H.pos[x]);
            CHECK(sz(subs) == sr - sl + 1 && *subs.begin() == sl && *subs.rbegin() == sr);
        }
        // centroid tree: each centroid splits its component into parts <= half
        { int roots = 0; rep(v, 0, tn) roots += C.cpar[v] == -1; CHECK(roots == 1); }
    }
    puts("graphs ok");
}
