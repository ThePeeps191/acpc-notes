// uses: 03-dp/01-dp-patterns.md lis countPalSubseq knap01 countWays
// uses: 03-dp/02-bitmask-digit-sos.md tsp sos countDigitSum dominoTilings
// uses: 03-dp/03-tree-dp.md rootTree minColouring sumDistAll
// uses: 03-dp/04-dp-optimizations.md LineContainer LiChao partitionDC minCostNonDecreasing
mt19937 rg(99);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
int main() {
    int it = 0;
    CHECK(dominoTilings(2, 2, MOD) == 2 && dominoTilings(3, 2, MOD) == 3 && dominoTilings(4, 3, MOD) == 11);
    CHECK(dominoTilings(4, 4, MOD) == 36 && dominoTilings(8, 8, MOD) == 12988816);
    CHECK(countPalSubseq("abcdb", MOD) == 8 && countPalSubseq("aaa", MOD) == 7 && countPalSubseq("bob", MOD) == 5);
    for (it = 0; it < 400; ++it) {
        int n = R(1, 11);
        string s; rep(i, 0, n) s += char('a' + R(0, 2));
        ll cnt = 0;
        rep(m, 1, 1 << n) { string t; rep(i, 0, n) if (m >> i & 1) t += s[i]; string u = t; reverse(all(u)); cnt += t == u; }
        CHECK(countPalSubseq(s, MOD) == cnt);
        vector<ll> a(n); for (auto& x : a) x = R(0, 6);
        vector<int> L(n, 1); int best = 0;
        rep(i, 0, n) { rep(j, 0, i) if (a[j] < a[i]) L[i] = max(L[i], L[j] + 1); best = max(best, L[i]); }
        CHECK(lis(a) == best);
        // knapsack vs brute
        vector<int> w(n); vector<ll> v(n); rep(i, 0, n) w[i] = R(1, 8), v[i] = R(0, 9);
        int W = R(0, 20); ll bk = 0;
        rep(m, 0, 1 << n) { int ww = 0; ll vv = 0; rep(i, 0, n) if (m >> i & 1) ww += w[i], vv += v[i]; if (ww <= W) bk = max(bk, vv); }
        CHECK(knap01(w, v, W) == bk);
        // slope trick vs dp over candidate values
        vector<ll> c = a; sort(all(c)); c.erase(unique(all(c)), c.end());
        vector<ll> dp(sz(c), 0);
        rep(i, 0, n) { ll run = INF; rep(j, 0, sz(c)) { run = min(run, dp[j]); dp[j] = run + llabs(a[i] - c[j]); } }
        CHECK(minCostNonDecreasing(a) == *min_element(all(dp)));
        // digit dp
        ll N = R(0, 3000); int K = R(1, 12); ll bd = 0;
        rep(x, 0, N + 1) { int ds = 0; for (int y = x; y; y /= 10) ds += y % 10; bd += ds % K == 0; }
        CHECK(countDigitSum(N, K) == bd);
        // sos
        int b = R(1, 6); vector<ll> A(1 << b), F; for (auto& x : A) x = R(0, 9); F = A; sos(F, b);
        rep(m, 0, 1 << b) { ll s2 = 0; rep(t, 0, 1 << b) if ((t & m) == t) s2 += A[t]; CHECK(F[m] == s2); }
        // tsp vs permutations
        int tn = R(1, 6); vector<vector<ll>> d(tn, vector<ll>(tn)); rep(i, 0, tn) rep(j, 0, tn) d[i][j] = i == j ? 0 : R(1, 20);
        vector<int> p(tn); iota(all(p), 0); ll bt = INF;
        do { if (p[0] != 0) break; ll cst = 0; rep(i, 0, tn) cst += d[p[i]][p[(i + 1) % tn]]; bt = min(bt, cst); } while (next_permutation(all(p)));
        CHECK(tsp(d) == (tn == 1 ? 0 : bt));
        // trees
        int tnn = R(1, 7); vector<vector<int>> g(tnn); rep(i, 1, tnn) { int q = R(0, i - 1); g[i].push_back(q); g[q].push_back(i); }
        int k = R(2, 3); ll bc = INF; vector<int> col(tnn);
        function<void(int)> go = [&](int i) {
            if (i == tnn) { ll cst = 0; rep(x, 0, tnn) { cst += col[x]; for (int y : g[x]) if (col[x] == col[y]) return; } bc = min(bc, cst); return; }
            rep(cc, 1, k + 1) { col[i] = cc; go(i + 1); }
        };
        go(0);
        CHECK(minColouring(g, k) == bc);
        auto sd = sumDistAll(g);
        rep(s0, 0, tnn) { vector<int> dist(tnn, -1); dist[s0] = 0; queue<int> q; q.push(s0); ll tot = 0;
            while (!q.empty()) { int x = q.front(); q.pop(); tot += dist[x]; for (int y : g[x]) if (dist[y] < 0) dist[y] = dist[x] + 1, q.push(y); }
            CHECK(sd[s0] == tot); }
        // CHT / Li Chao
        LineContainer lc; vector<ll> qx; rep(i, 0, 30) qx.push_back(R(-50, 50));
        LiChao li(qx); vector<pll> lines;
        rep(i, 0, 25) {
            ll kk = R(-20, 20), mm = R(-100, 100); lines.push_back({kk, mm}); lc.add(kk, mm); li.add(kk, mm);
            ll x = qx[R(0, 29)], mx = LLONG_MIN, mn = LLONG_MAX;
            for (auto [a1, b1] : lines) mx = max(mx, a1 * x + b1), mn = min(mn, a1 * x + b1);
            CHECK(lc.query(x) == mx); CHECK(li.query(x) == mn);
        }
        // D&C partition with cost = (segment sum)^2
        int pn = R(1, 12), K2 = R(1, pn); vector<ll> pa(pn), pre(pn + 1, 0); rep(i, 0, pn) pa[i] = R(1, 9), pre[i + 1] = pre[i] + pa[i];
        auto cost = [&](int j, int i) { ll s2 = pre[i] - pre[j]; return s2 * s2; };
        vector<vector<ll>> bd2(K2 + 1, vector<ll>(pn + 1, INF)); bd2[0][0] = 0;
        rep(kk, 1, K2 + 1) rep(i, 1, pn + 1) rep(j, 0, i) if (bd2[kk - 1][j] < INF) bd2[kk][i] = min(bd2[kk][i], bd2[kk - 1][j] + cost(j, i));
        CHECK(partitionDC(pn, K2, cost) == bd2[K2][pn]);
        // countWays: compare to dp counting
        vector<int> coins = {R(1, 4), R(1, 6)}; if (coins[0] == coins[1]) coins.pop_back(); int S = R(0, 20); ll cw = 0;
        rep(x, 0, S + 1) rep(y, 0, S + 1) if (sz(coins) == 2 ? x * coins[0] + y * coins[1] == S : (y == 0 && x * coins[0] == S)) cw++;
        CHECK(countWays(coins, S, MOD) == cw);
    }
    puts("dp ok");
}
