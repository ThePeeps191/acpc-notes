# Convex Hull, Calipers, Closest Pair, Circles, Integration

Needs `P`, `Pt`, `Pd` from §9.1.

## Convex hull (monotone chain)

```cpp
// [stress-tested] counter-clockwise, no collinear points, no duplicate first/last. Handles n <= 2.
vector<Pt> convexHull(vector<Pt> pts) {
    if (sz(pts) <= 1) return pts;
    sort(all(pts));
    vector<Pt> h(sz(pts) + 1);
    int s = 0, t = 0;
    for (int it = 2; it--; s = --t, reverse(all(pts)))
        for (Pt p : pts) {
            while (t >= s + 2 && h[t - 2].cross(h[t - 1], p) <= 0) t--;   // < 0 keeps collinear points
            h[t++] = p;
        }
    return {h.begin(), h.begin() + t - (t == 2 && h[0] == h[1])};
}
```

- Hull perimeter/area: sum edge lengths / `area2` of the hull.
- Keeping collinear boundary points (`< 0`) breaks when *all* points are collinear (returns duplicates) — handle that case separately.
- **Online upper hull** for CHT-like problems → §4.4. **Dynamic hull** is rarely needed.
- Fractal/self-similar shapes (ACPC 2020 *Wrapping Trees*): the hull of the limit set is determined by extreme points of the generator; reason about which corners survive.

## Rotating calipers

```cpp
// [stress-tested] squared diameter (farthest pair) of a convex polygon given CCW (output of convexHull)
ll diameter2(const vector<Pt>& h) {
    int n = sz(h);
    if (n == 1) return 0;
    if (n == 2) return (h[0] - h[1]).norm2();
    ll best = 0;
    for (int i = 0, j = 1; i < n; ++i) {
        while ((h[(i + 1) % n] - h[i]).cross(h[(j + 1) % n] - h[j]) > 0) j = (j + 1) % n;
        best = max({best, (h[i] - h[j]).norm2(), (h[(i + 1) % n] - h[j]).norm2()});
    }
    return best;
}
```

Same two-pointer skeleton gives: minimum width (min over edges of max distance to the antipodal vertex), smallest enclosing rectangle, max distance between two convex polygons.

## Closest pair of points

```cpp
// [stress-tested] squared distance of the closest pair, O(n log n)
ll closestPair2(vector<Pt> p) {
    sort(all(p), [](Pt a, Pt b) { return a.x < b.x; });
    set<pair<ll, ll>> S; ll best = LLONG_MAX; int j = 0;
    rep(i, 0, sz(p)) {
        ll d = (ll)ceil(sqrt((long double)best));
        while (p[i].x - p[j].x > d) { S.erase({p[j].y, p[j].x}); ++j; }
        auto lo = S.lower_bound({p[i].y - d, LLONG_MIN}), hi = S.upper_bound({p[i].y + d, LLONG_MAX});
        for (auto it = lo; it != hi; ++it) {
            ll dx = p[i].x - it->second, dy = p[i].y - it->first;
            best = min(best, dx * dx + dy * dy);
        }
        S.insert({p[i].y, p[i].x});
    }
    return best;
}
```

## Circles

```cpp
// [stress-tested] circumcentre of triangle ABC (non-collinear)
Pd ccCenter(Pd A, Pd B, Pd C) {
    Pd b = C - A, c = B - A;
    return A + (b * c.norm2() - c * b.norm2()).perp() / b.cross(c) / 2;
}
// [stress-tested] minimum enclosing circle, expected O(n) (Welzl, randomised)
pair<Pd, double> minEnclosingCircle(vector<Pd> ps) {
    shuffle(all(ps), mt19937(1));
    Pd o = ps[0]; double r = 0, EPS = 1 + 1e-9;
    rep(i, 0, sz(ps)) if ((o - ps[i]).len() > r * EPS) {
        o = ps[i]; r = 0;
        rep(j, 0, i) if ((o - ps[j]).len() > r * EPS) {
            o = (ps[i] + ps[j]) / 2; r = (o - ps[i]).len();
            rep(k, 0, j) if ((o - ps[k]).len() > r * EPS) {
                o = ccCenter(ps[i], ps[j], ps[k]); r = (o - ps[i]).len();
            }
        }
    }
    return {o, r};
}
// intersections of circle (c, r) with the infinite line (a, b)
vector<Pd> circleLine(Pd c, double r, Pd a, Pd b) {
    Pd ab = b - a, p = a + ab * ((c - a).dot(ab) / ab.norm2());
    double s = a.cross(b, c), h2 = r * r - s * s / ab.norm2();
    if (h2 < 0) return {};
    if (h2 == 0) return {p};
    Pd h = ab / ab.len() * sqrt(h2);
    return {p - h, p + h};
}
// intersections of two circles (0, 1 or 2 points); identical circles -> {}
vector<Pd> circleCircle(Pd a, double r1, Pd b, double r2) {
    Pd v = b - a; double d2 = v.norm2();
    if (d2 == 0) return {};
    double sum = r1 + r2, dif = r1 - r2;
    if (sum * sum < d2 || dif * dif > d2) return {};
    double p = (d2 + r1 * r1 - r2 * r2) / (2 * d2), h2 = r1 * r1 - p * p * d2;
    Pd mid = a + v * p, per = v.perp() * sqrt(max(0.0, h2) / d2);
    if (h2 <= 0) return {mid};
    return {mid + per, mid - per};
}
```

- Tangent length from external point P: √(|PC|² − r²). Angle subtended: 2·asin(r / |PC|).
- Area of intersection of two circles with distance d: `r1²·acos((d² + r1² − r2²)/(2 d r1)) + r2²·acos((d² + r2² − r1²)/(2 d r2)) − ½√((−d+r1+r2)(d+r1−r2)(d−r1+r2)(d+r1+r2))` (handle containment / disjoint first).
- Circle through 3 points: `ccCenter`. Point inside circle: compare squared distances.
- Moving circle / point vs shrinking zone (ACPC 2018 *PUBNite*): write distance as a function of time, binary/ternary search the time, or solve the quadratic directly.

## Sphere / 3-D quick facts

- Great-circle distance between points with unit vectors u, v on a sphere radius R: `R·acos(u·v)` (clamp the dot). Chord length `2R·sin(θ/2)`.
- Lat/long → xyz: (cos φ cos λ, cos φ sin λ, sin φ).
- Moving along a tangent line a distance s from the surface point: distance to centre √(R² + s²) (ACPC 2023 *Flying Straight* compares arc vs tangent).
- 3-D cross product `(a.y b.z − a.z b.y, a.z b.x − a.x b.z, a.x b.y − a.y b.x)`; volume of tetrahedron = |a·(b×c)|/6.

## Numerical integration and root finding

```cpp
// composite Simpson on [a, b] with n (even) intervals; error O(h^4)
template <class F> double simpson(F f, double a, double b, int n = 2000) {
    double h = (b - a) / n, s = f(a) + f(b);
    rep(i, 1, n) s += f(a + i * h) * (i & 1 ? 4 : 2);
    return s * h / 3;
}
```

- Area of a union of shapes: integrate the covered length of the vertical line x = t (sweep events at all intersection x-coordinates, Simpson within each slab).
- Roots of a monotone function: binary search (§2.1). Polynomial root isolation: roots of the derivative split the axis into monotone pieces.
