// uses: 05-flows-matching/01-max-flow.md Dinic minFlowCoverEdges
// uses: 05-flows-matching/02-bipartite-matching.md HopcroftKarp
// uses: 05-flows-matching/03-min-cost-assignment.md hungarian MCMF
mt19937 rg(777);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
int main() {
    for (int it = 0; it < 1500; ++it) {
        // ---- max flow vs brute min cut
        int n = R(2, 7), m = R(0, 14), s = 0, t = n - 1;
        vector<array<int, 3>> E; Dinic d(n);
        rep(i, 0, m) { int u = R(0, n - 1), v = R(0, n - 1), c = R(0, 9); if (u == v) continue; E.push_back({u, v, c}); d.addEdge(u, v, c); }
        ll f = d.maxflow(s, t), best = INF;
        rep(mask, 0, 1 << n) { if (!(mask & 1) || (mask >> t & 1)) continue; ll c = 0; for (auto [u, v, w] : E) if ((mask >> u & 1) && !(mask >> v & 1)) c += w; best = min(best, c); }
        CHECK(f == best);
        ll cutc = 0; for (auto [u, v, w] : E) if (d.inS(u) && !d.inS(v)) cutc += w; CHECK(cutc == f);
        // ---- min flow covering all edges of a DAG (every edge on an s-t path)
        {
            int dn = R(2, 6); vector<pii> de;
            rep(u, 0, dn) rep(v, u + 1, dn) if (R(0, 2) == 0) de.push_back({u, v});
            // keep only edges on some 0 -> dn-1 path
            vector<char> fromS(dn), toT(dn); fromS[0] = 1; toT[dn - 1] = 1;
            rep(u, 0, dn) for (auto [a, b] : de) if (a == u && fromS[a]) fromS[b] = 1;
            for (int u = dn - 1; u >= 0; --u) for (auto [a, b] : de) if (a == u && toT[b]) toT[a] = 1;
            vector<pii> ok; for (auto [a, b] : de) if (fromS[a] && toT[b]) ok.push_back({a, b});
            if (!ok.empty() && sz(ok) <= 10) {
                // enumerate all s-t paths as edge masks
                vector<int> paths;
                function<void(int, int)> go = [&](int v, int mask) { if (v == dn - 1) { paths.push_back(mask); return; }
                    rep(i, 0, sz(ok)) if (ok[i].first == v) go(ok[i].second, mask | 1 << i); };
                go(0, 0);
                int full = (1 << sz(ok)) - 1, ans = -1;
                vector<int> dp(1 << sz(ok), 1e9); dp[0] = 0;
                rep(mask, 0, 1 << sz(ok)) if (dp[mask] < 1e9) for (int p : paths) dp[mask | p] = min(dp[mask | p], dp[mask] + 1);
                ans = dp[full];
                CHECK(minFlowCoverEdges(dn, ok, 0, dn - 1) == ans);
            }
        }
        // ---- bipartite matching
        int L = R(1, 6), Rn = R(1, 6); HopcroftKarp hk(L, Rn); vector<vector<int>> adj(L, vector<int>(Rn, 0));
        rep(u, 0, L) rep(v, 0, Rn) if (R(0, 2) == 0) { hk.addEdge(u, v); adj[u][v] = 1; }
        int mm = hk.maxMatching();
        // brute: dp over left with mask of used right
        vector<int> dpm(1 << Rn, -1); dpm[0] = 0;
        rep(u, 0, L) { vector<int> nd = dpm; rep(mask, 0, 1 << Rn) if (dpm[mask] >= 0) rep(v, 0, Rn) if (adj[u][v] && !(mask >> v & 1)) nd[mask | 1 << v] = max(nd[mask | 1 << v], dpm[mask] + 1); dpm = nd; }
        CHECK(mm == *max_element(all(dpm)));
        auto [cl, cr] = hk.minVertexCover(); CHECK(sz(cl) + sz(cr) == mm);
        { set<int> SL(all(cl)), SR(all(cr)); rep(u, 0, L) rep(v, 0, Rn) if (adj[u][v]) CHECK(SL.count(u) || SR.count(v)); }
        // ---- hungarian vs permutations, MCMF vs hungarian
        int hn = R(1, 5), hm = R(hn, 6); vector<vector<ll>> a(hn, vector<ll>(hm)); for (auto& row : a) for (auto& x : row) x = R(-20, 20);
        auto [hc, match] = hungarian(a);
        vector<int> cols(hm); iota(all(cols), 0); ll bb = INF;
        do { ll c = 0; rep(i, 0, hn) c += a[i][cols[i]]; bb = min(bb, c); } while (next_permutation(all(cols)));
        CHECK(hc == bb);
        { ll c = 0; set<int> used; rep(i, 0, hn) { c += a[i][match[i]]; used.insert(match[i]); } CHECK(c == hc && sz(used) == hn); }
        MCMF mc(hn + hm + 2); int S = hn + hm, T = S + 1;
        rep(i, 0, hn) { mc.addEdge(S, i, 1, 0); rep(j, 0, hm) mc.addEdge(i, hn + j, 1, a[i][j]); }
        rep(j, 0, hm) mc.addEdge(hn + j, T, 1, 0);
        auto [fl, cost] = mc.run(S, T); CHECK(fl == hn && cost == hc);
        // any-amount min cost (max weight matching, not necessarily perfect)
        MCMF mc2(hn + hm + 2);
        rep(i, 0, hn) { mc2.addEdge(S, i, 1, 0); rep(j, 0, hm) mc2.addEdge(i, hn + j, 1, a[i][j]); }
        rep(j, 0, hm) mc2.addEdge(hn + j, T, 1, 0);
        auto [fl2, cost2] = mc2.run(S, T, INF, true);
        ll b2 = 0; // brute: choose any matching minimising cost
        function<void(int, int, ll)> go2 = [&](int i, int usedMask, ll c) { if (i == hn) { b2 = min(b2, c); return; } go2(i + 1, usedMask, c);
            rep(j, 0, hm) if (!(usedMask >> j & 1)) go2(i + 1, usedMask | 1 << j, c + a[i][j]); };
        go2(0, 0, 0); CHECK(cost2 == b2);
    }
    puts("flows ok");
}
