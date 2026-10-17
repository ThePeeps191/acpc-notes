// uses: 02-data-structures/02-fenwick.md BIT BITRangeAdd
// uses: 04-graphs/01-shortest-paths.md gridBFS
// uses: 04-graphs/04-trees.md Centroid
// uses: 08-geometry/01-primitives.md P onSegment segIntersect lineIntersection
// uses: 08-geometry/02-hull-circles-misc.md ccCenter circleLine circleCircle
mt19937 rg(99);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
int main() {
    for (int it = 0; it < 3000; ++it) {
        // BITRangeAdd
        { int n = R(1, 20); BITRangeAdd b(n); vector<ll> a(n, 0);
          rep(q, 0, 30) { int l = R(0, n - 1), r = R(l, n - 1); ll v = R(-5, 5); b.add(l, r, v); rep(i, l, r + 1) a[i] += v; int i = R(0, n - 1); CHECK(b.get(i) == a[i]); } }
        // gridBFS vs relaxation
        { int Rr = R(1, 6), C = R(1, 6); vector<string> g(Rr, string(C, '.')); rep(i, 0, Rr) rep(j, 0, C) if (R(0, 3) == 0) g[i][j] = '#';
          vector<pii> src; rep(k, 0, R(1, 3)) { int r = R(0, Rr - 1), c = R(0, C - 1); if (g[r][c] == '.') src.push_back({r, c}); }
          if (!src.empty()) { auto d = gridBFS(g, src); vector<vector<int>> b(Rr, vector<int>(C, 1e9)); for (auto [r, c] : src) b[r][c] = 0;
            rep(rep_, 0, Rr * C + 2) rep(i, 0, Rr) rep(j, 0, C) if (g[i][j] == '.') rep(k, 0, 4) { int ni = i + DR[k], nj = j + DC[k]; if (ni >= 0 && ni < Rr && nj >= 0 && nj < C && g[ni][nj] == '.') b[i][j] = min(b[i][j], b[ni][nj] + 1); }
            rep(i, 0, Rr) rep(j, 0, C) CHECK(d[i][j] == (b[i][j] >= 1e9 ? -1 : b[i][j])); } }
        // centroid decomposition property
        { int n = R(1, 15); vector<vector<int>> g(n); rep(i, 1, n) { int p = R(0, i - 1); g[i].push_back(p); g[p].push_back(i); }
          Centroid C(g);
          rep(c, 0, n) {
              vector<char> in(n, 0); rep(v, 0, n) { int x = v; while (x != -1 && x != c) x = C.cpar[x]; in[v] = x == c; }
              int tot = count(all(in), 1);
              // connected + each part after removing c has size <= tot/2
              vector<char> seen(n, 0); function<int(int)> dfs = [&](int v) { seen[v] = 1; int s = 1; for (int u : g[v]) if (in[u] && !seen[u] && u != c) s += dfs(u); return s; };
              seen[c] = 1; int sum = 1; for (int u : g[c]) if (in[u] && !seen[u]) { int s = dfs(u); CHECK(2 * s <= tot); sum += s; }
              CHECK(sum == tot);
          } }
        // segment intersection vs sampling-free brute (rational check)
        { Pt a(R(-4, 4), R(-4, 4)), b(R(-4, 4), R(-4, 4)), c(R(-4, 4), R(-4, 4)), d(R(-4, 4), R(-4, 4));
          bool got = segIntersect(a, b, c, d), brute = false;
          // brute: check points on a fine rational grid along ab (denominator 120) lying on cd
          rep(t, 0, 121) { ll X = a.x * 120 + (b.x - a.x) * t, Y = a.y * 120 + (b.y - a.y) * t; Pt q(X, Y);
              Pt c2(c.x * 120, c.y * 120), d2(d.x * 120, d.y * 120); if (onSegment(c2, d2, q)) brute = true; }
          if (!brute) { // intersection may have denominator not dividing 120: compute exactly with cross products
              ll den = (b - a).cross(d - c);
              if (den != 0) { ll tn = (c - a).cross(d - c), un = (c - a).cross(b - a);
                  if (den < 0) den = -den, tn = -tn, un = -un;
                  if (tn >= 0 && tn <= den && un >= 0 && un <= den) brute = true; } }
          CHECK(got == brute);
          ll den = (b - a).cross(d - c);
          if (den != 0) { Pd p = lineIntersection(Pd(a.x, a.y), Pd(b.x, b.y), Pd(c.x, c.y), Pd(d.x, d.y));
              CHECK(fabs(Pd(a.x, a.y).cross(Pd(b.x, b.y), p)) < 1e-6 && fabs(Pd(c.x, c.y).cross(Pd(d.x, d.y), p)) < 1e-6); } }
        // circles
        { Pd c(R(-5, 5), R(-5, 5)); double r = R(1, 5); Pd a(R(-6, 6), R(-6, 6)), b(R(-6, 6), R(-6, 6)); if (a == b) continue;
          auto v = circleLine(c, r, a, b); for (auto p : v) { CHECK(fabs((p - c).len() - r) < 1e-6); CHECK(fabs(a.cross(b, p)) < 1e-6); }
          double dist = fabs(a.cross(b, c)) / (b - a).len(); if (dist < r - 1e-9) CHECK(sz(v) == 2); if (dist > r + 1e-9) CHECK(sz(v) == 0);
          Pd c2(R(-5, 5), R(-5, 5)); double r2 = R(1, 5); auto w = circleCircle(c, r, c2, r2);
          for (auto p : w) { CHECK(fabs((p - c).len() - r) < 1e-6 && fabs((p - c2).len() - r2) < 1e-6); }
          double dd = (c2 - c).len(); if (dd > 0 && dd < r + r2 - 1e-9 && dd > fabs(r - r2) + 1e-9) CHECK(sz(w) == 2);
          if (dd > r + r2 + 1e-9 || dd < fabs(r - r2) - 1e-9) CHECK(sz(w) == 0); }
    }
    puts("extra2 ok");
}
