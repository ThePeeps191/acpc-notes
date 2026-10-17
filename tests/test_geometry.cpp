// uses: 08-geometry/01-primitives.md P onSegment polarSort area2 inPolygon distPointSegment
// uses: 08-geometry/02-hull-circles-misc.md convexHull diameter2 closestPair2 ccCenter minEnclosingCircle
mt19937 rg(11);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
int main() {
    for (int it = 0; it < 3000; ++it) {
        int n = R(1, 12), C = R(1, 6); vector<Pt> pts(n); for (auto& p : pts) p = Pt(R(-C, C), R(-C, C));
        auto h = convexHull(pts);
        // every point inside or on hull; hull strictly convex & CCW
        int m = sz(h);
        if (m >= 3) {
            rep(i, 0, m) CHECK(h[i].cross(h[(i + 1) % m], h[(i + 2) % m]) > 0);
            for (auto p : pts) CHECK(inPolygon(h, p) != 0);
            CHECK(area2(h) > 0);
        }
        // hull vertices are input points, brute: a point is a hull vertex iff not in the convex combination... check count via extreme directions
        ll bd = 0; for (auto a : pts) for (auto b : pts) bd = max(bd, (a - b).norm2());
        CHECK(diameter2(h) == bd);
        if (n >= 2) { ll bc = LLONG_MAX; rep(i, 0, n) rep(j, i + 1, n) bc = min(bc, (pts[i] - pts[j]).norm2()); CHECK(closestPair2(pts) == bc); }
        // point in polygon vs brute on a random simple polygon (star-shaped around origin via polar sort)
        vector<Pt> poly; set<pair<ll,ll>> seen;
        rep(i, 0, R(3, 8)) { Pt p(R(-6, 6), R(-6, 6)); if (p == Pt(0, 0) || seen.count({p.x, p.y})) continue; seen.insert({p.x, p.y}); poly.push_back(p); }
        polarSort(poly);
        bool okPoly = sz(poly) >= 3;
        rep(i, 0, sz(poly)) if (okPoly) { Pt a = poly[i], b = poly[(i + 1) % sz(poly)]; if (a.cross(b) <= 0) okPoly = false; }   // strictly star-shaped CCW
        if (okPoly) {
            rep(q, 0, 20) {
                Pt p(R(-7, 7), R(-7, 7)); int res = inPolygon(poly, p);
                bool onB = false; rep(i, 0, sz(poly)) onB |= onSegment(poly[i], poly[(i + 1) % sz(poly)], p);
                // brute inside: p inside some fan triangle (0, a, b) strictly or on internal fan edges
                bool inside = false;
                rep(i, 0, sz(poly)) { Pt a = poly[i], b = poly[(i + 1) % sz(poly)], o(0, 0);
                    ll c1 = o.cross(a, p), c2 = a.cross(b, p), c3 = b.cross(o, p);
                    if (c1 >= 0 && c2 >= 0 && c3 >= 0) inside = true; }
                if (onB) { CHECK(res == -1); } else { CHECK(res == (inside ? 1 : 0)); }
            }
        }
        // polar sort monotone angles
        { vector<Pt> v; rep(i, 0, 10) { Pt p(R(-5, 5), R(-5, 5)); if (!(p == Pt(0, 0))) v.push_back(p); } polarSort(v);
          rep(i, 1, sz(v)) { double a1 = v[i - 1].angle(), a2 = v[i].angle(); if (a1 < 0) a1 += 2 * M_PI; if (a2 < 0) a2 += 2 * M_PI; CHECK(a1 <= a2 + 1e-12); } }
        // min enclosing circle: contains all points, and is no bigger than brute candidate minimum
        { vector<Pd> ps; for (auto p : pts) ps.push_back(Pd(p.x, p.y)); auto [o, r] = minEnclosingCircle(ps);
          for (auto p : ps) CHECK((p - o).len() <= r + 1e-7);
          double br = 1e18; int k = sz(ps);
          rep(i, 0, k) rep(j, i, k) { Pd c = (ps[i] + ps[j]) / 2; double rr = 0; for (auto p : ps) rr = max(rr, (p - c).len()); br = min(br, rr);
              rep(l, j + 1, k) { if (fabs((ps[j] - ps[i]).cross(ps[l] - ps[i])) < 1e-12) continue; Pd c2 = ccCenter(ps[i], ps[j], ps[l]); double r2 = 0; for (auto p : ps) r2 = max(r2, (p - c2).len()); br = min(br, r2); } }
          CHECK(fabs(br - r) < 1e-6); }
        // distPointSegment vs sampling
        { Pd a(R(-5, 5), R(-5, 5)), b(R(-5, 5), R(-5, 5)), p(R(-5, 5), R(-5, 5)); double d = distPointSegment(a, b, p), bb = 1e18;
          rep(s, 0, 2001) { Pd q = a + (b - a) * (s / 2000.0); bb = min(bb, (p - q).len()); } CHECK(d <= bb + 1e-9 && bb - d < 1e-2); }
    }
    puts("geometry ok");
}
