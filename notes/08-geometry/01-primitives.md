# Geometry Primitives

**Rule 1: stay in integers as long as possible.** Inputs are usually integers; cross/dot products of integer points are exact in `ll` (|coords| ≤ 10^9 ⇒ products ≤ 4·10^18: borderline, use `__int128` for cross products of differences if coordinates reach 10^9). Only divide/sqrt at the very end. **Rule 2:** with doubles, compare with `EPS` (1e-9 relative to the scale of the input).

## Point / vector

```cpp
// [stress-tested]
template <class T> struct P {
    T x, y;
    P(T x = 0, T y = 0) : x(x), y(y) {}
    P operator+(P o) const { return {x + o.x, y + o.y}; }
    P operator-(P o) const { return {x - o.x, y - o.y}; }
    P operator*(T k) const { return {x * k, y * k}; }
    P operator/(T k) const { return {x / k, y / k}; }
    bool operator<(P o) const { return tie(x, y) < tie(o.x, o.y); }
    bool operator==(P o) const { return x == o.x && y == o.y; }
    T dot(P o) const { return x * o.x + y * o.y; }
    T cross(P o) const { return x * o.y - y * o.x; }
    T cross(P a, P b) const { return (a - *this).cross(b - *this); }   // >0: a->b turns left around *this
    T norm2() const { return x * x + y * y; }
    double len() const { return sqrt((double)norm2()); }
    double angle() const { return atan2((double)y, (double)x); }        // (-pi, pi]
    P perp() const { return {-y, x}; }                                  // rotate +90°
    P<double> rotate(double a) const { return {x * cos(a) - y * sin(a), x * sin(a) + y * cos(a)}; }
};
using Pt = P<ll>; using Pd = P<double>;
template <class T> int sgn(T v) { return (v > 0) - (v < 0); }
// orientation of c relative to line a->b: +1 left (CCW), -1 right (CW), 0 collinear
template <class T> int orient(P<T> a, P<T> b, P<T> c) { return sgn(a.cross(b, c)); }
```

## Segments and lines

```cpp
// [stress-tested] is p on segment [a, b] (integers, exact)
bool onSegment(Pt a, Pt b, Pt p) {
    return a.cross(b, p) == 0 && min(a.x, b.x) <= p.x && p.x <= max(a.x, b.x)
                              && min(a.y, b.y) <= p.y && p.y <= max(a.y, b.y);
}
// do closed segments [a,b] and [c,d] intersect (touching counts)
bool segIntersect(Pt a, Pt b, Pt c, Pt d) {
    int o1 = orient(a, b, c), o2 = orient(a, b, d), o3 = orient(c, d, a), o4 = orient(c, d, b);
    if (o1 * o2 < 0 && o3 * o4 < 0) return true;                       // proper crossing
    return onSegment(a, b, c) || onSegment(a, b, d) || onSegment(c, d, a) || onSegment(c, d, b);
}
// intersection point of infinite lines (a,b) and (c,d); requires non-parallel
Pd lineIntersection(Pd a, Pd b, Pd c, Pd d) {
    double t = (c - a).cross(d - c) / (b - a).cross(d - c);
    return a + (b - a) * t;
}
// distance from p to segment [a, b]
double distPointSegment(Pd a, Pd b, Pd p) {
    Pd ab = b - a; double L = ab.norm2();
    if (L == 0) return (p - a).len();
    double t = max(0.0, min(1.0, (p - a).dot(ab) / L));
    return (p - (a + ab * t)).len();
}
// projection of p onto the line (a, b); reflection = 2 * proj - p
Pd projectLine(Pd a, Pd b, Pd p) { Pd ab = b - a; return a + ab * ((p - a).dot(ab) / ab.norm2()); }
```

- Distance from p to the infinite line (a, b): `|cross(b − a, p − a)| / |b − a|`.
- Segment–segment distance: 0 if they intersect, else min of the 4 point–segment distances.
- **Lattice points** on segment (a, b) inclusive: `gcd(|dx|, |dy|) + 1`.
- Line through two points as `A x + B y = C`: A = y2 − y1, B = x1 − x2, C = A·x1 + B·y1 (integers).
- Parallel ⇔ cross(b − a, d − c) == 0; perpendicular ⇔ dot == 0.

## Angles and sorting by angle

```cpp
// [stress-tested] sort vectors by polar angle in [0, 2pi), exact for integers; ties by distance not handled
bool half(Pt p) { return p.y < 0 || (p.y == 0 && p.x < 0); }
void polarSort(vector<Pt>& v) {
    sort(all(v), [](Pt a, Pt b) {
        if (half(a) != half(b)) return half(a) < half(b);
        return a.cross(b) > 0;
    });
}
```

- Angle between vectors: `atan2(cross, dot)` (signed, robust) rather than `acos(dot/(|a||b|))` (inaccurate near 0, π).
- Rotating sweep around a point (max points visible in an angle window): polar sort, double the array with angles + 2π, two pointers.

## Polygons

```cpp
// [stress-tested] twice the signed area (positive if counter-clockwise). Exact in integers.
ll area2(const vector<Pt>& poly) {
    ll a = 0; int n = sz(poly);
    rep(i, 0, n) a += poly[i].cross(poly[(i + 1) % n]);
    return a;
}
// point in simple polygon: 1 inside, 0 outside, -1 on boundary. O(n), exact.
int inPolygon(const vector<Pt>& poly, Pt p) {
    int n = sz(poly); bool in = false;
    rep(i, 0, n) {
        Pt a = poly[i], b = poly[(i + 1) % n];
        if (onSegment(a, b, p)) return -1;
        if ((a.y > p.y) != (b.y > p.y)) {               // edge crosses the horizontal line y = p.y
            __int128 c = (__int128)(b - a).cross(p - a);
            if ((c > 0) == (b.y > a.y)) in = !in;       // p is strictly left of an upward edge, etc.
        }
    }
    return in ? 1 : 0;
}
```

- **Pick's theorem** (lattice polygon): Area = I + B/2 − 1 ⇒ interior points I = (area2 − B + 2) / 2 with B = Σ gcd(|dx|, |dy|) over edges.
- Centroid of polygon: C = Σ (p_i + p_{i+1})·cross(p_i, p_{i+1}) / (3·area2).
- **Point in convex polygon in O(log n):** binary search the fan triangle from vertex 0 (`orient` with poly[0], poly[mid]).
- Polygon is convex ⇔ all consecutive turns have the same sign (ignore zero for collinear).
- Area of a polygon cut by a half-plane: walk edges, keep inside vertices + intersection points (Sutherland–Hodgman), O(n).
- **Polar area** (curve r(θ)): area = ∫ r(θ)²/2 dθ (ACPC 2021 *Wonky Pizza*: r linear in θ → closed-form cubic; then binary search the cut angle).

## Precision checklist

- Use `long double` if values ~10^9 and you need 1e-6 after subtractive operations.
- Never test `==` on doubles; `fabs(a − b) < EPS`. Choose EPS by scale: coordinates 10^4 → EPS 1e-9 fine; 10^9 → use relative error.
- `sqrt` of a slightly negative number → NaN: `sqrt(max(0.0, x))`. Same for `acos(min(1.0, max(-1.0, x)))`.
- Print `-0.000` as `0.000`: add `0.0` or check `fabs(x) < 5e-10`.
- Integer geometry: test collinearity / orientation exactly; compare distances by squared lengths.
