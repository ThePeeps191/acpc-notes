// uses: 04-graphs/01-shortest-paths.md bipartite findCycle
// uses: 06-math/04-linear-algebra.md detMod lagrange
// uses: 06-math/01-modular.md discreteLog
// uses: 02-data-structures/03-segment-tree.md PST
// uses: 08-geometry/01-primitives.md P onSegment area2 inPolygon inConvex polygonCut
// uses: 08-geometry/02-hull-circles-misc.md convexHull
mt19937 rg(8);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
ll pw(ll b, ll e, ll m) { ll r = 1 % m; b %= m; for (; e; e >>= 1, b = b * b % m) if (e & 1) r = r * b % m; return r; }
int main() {
    for (int it = 0; it < 2000; ++it) {
        int n = R(1, 7);
        // bipartite vs brute 2-colouring
        vector<vector<int>> g(n); vector<pii> E; rep(i, 0, R(0, 10)) { int u = R(0, n - 1), v = R(0, n - 1); if (u == v) continue; g[u].push_back(v); g[v].push_back(u); E.push_back({u, v}); }
        bool bb = false; rep(mask, 0, 1 << n) { bool ok = true; for (auto [u, v] : E) if ((mask >> u & 1) == (mask >> v & 1)) ok = false; bb |= ok; }
        vector<int> col; bool got = bipartite(g, col); CHECK(got == bb);
        if (got) for (auto [u, v] : E) CHECK(col[u] != col[v]);
        // directed cycle
        vector<vector<int>> dg(n); vector<vector<int>> reach(n, vector<int>(n, 0));
        rep(i, 0, R(0, 10)) { int u = R(0, n - 1), v = R(0, n - 1); dg[u].push_back(v); reach[u][v] = 1; }
        rep(k, 0, n) rep(i, 0, n) rep(j, 0, n) if (reach[i][k] && reach[k][j]) reach[i][j] = 1;
        bool cyc = false; rep(i, 0, n) cyc |= reach[i][i];
        auto cy = findCycle(dg); CHECK(cy.empty() != cyc);
        rep(i, 0, sz(cy)) { int u = cy[i], v = cy[(i + 1) % sz(cy)]; CHECK(find(all(dg[u]), v) != dg[u].end()); }
        // determinant mod p vs permutation expansion
        { int m = R(1, 5); ll p = 1000000007; vector<vector<ll>> a(m, vector<ll>(m)); for (auto& r : a) for (auto& x : r) x = R(-5, 5);
          vector<int> perm(m); iota(all(perm), 0); ll det = 0;
          do { ll t = 1; rep(i, 0, m) t *= a[i][perm[i]]; int inv = 0; rep(i, 0, m) rep(j, i + 1, m) inv += perm[i] > perm[j]; det += inv & 1 ? -t : t; } while (next_permutation(all(perm)));
          CHECK(detMod(a, p) == ((det % p) + p) % p); }
        // lagrange: sum of i^k
        { int k = R(0, 6); ll p = 998244353; vector<ll> y(k + 2); ll s = 0; rep(i, 0, k + 2) { if (i) s = (s + pw(i, k, p)) % p; y[i] = s; }
          ll x = R(0, 300); ll bs = 0; rep(i, 1, x + 1) bs = (bs + pw(i, k, p)) % p; CHECK(lagrange(y, x, p) == bs); }
        // discrete log vs brute
        { ll m = R(2, 400), a = R(1, 400) % m, b = R(0, 400) % m; if (__gcd(a, m) == 1) { ll bx = -1, cur = 1 % m; rep(x, 0, 2 * m + 2) { if (cur == b) { bx = x; break; } cur = cur * a % m; } CHECK(discreteLog(a, b, m) == bx); } }
        // persistent segtree k-th
        { int len = R(1, 12), V = R(1, 8); vector<int> a(len); for (auto& x : a) x = R(0, V - 1); PST pst(V); vector<int> root(len + 1, 0);
          rep(i, 0, len) root[i + 1] = pst.insert(root[i], a[i]);
          rep(q, 0, 10) { int l = R(0, len - 1), r = R(l, len - 1), k = R(1, r - l + 1); vector<int> b(a.begin() + l, a.begin() + r + 1); sort(all(b)); CHECK(pst.kth(root[l], root[r + 1], k) == b[k - 1]); } }
        // inConvex vs inPolygon; polygonCut areas
        { vector<Pt> pts; rep(i, 0, R(3, 10)) pts.push_back(Pt(R(-6, 6), R(-6, 6))); auto h = convexHull(pts);
          if (sz(h) >= 3) {
              rep(q, 0, 20) { Pt p(R(-7, 7), R(-7, 7)); CHECK(inConvex(h, p) == (inPolygon(h, p) != 0)); }
              vector<Pd> hd; for (auto q : h) hd.push_back(Pd(q.x, q.y));
              Pd s(R(-7, 7), R(-7, 7)), e(R(-7, 7), R(-7, 7)); if (s == e) continue;
              auto L = polygonCut(hd, s, e), Rr = polygonCut(hd, e, s);
              auto ar = [](const vector<Pd>& v) { double A = 0; rep(i, 0, sz(v)) A += v[i].cross(v[(i + 1) % sz(v)]); return A; };
              CHECK(fabs(ar(L) + ar(Rr) - (double)area2(h)) < 1e-6);
              for (auto q : L) CHECK(s.cross(e, q) >= -1e-9);
          } }
    }
    puts("extra ok");
}
