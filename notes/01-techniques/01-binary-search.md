# Binary Search, Ternary Search, Parametric Search

## Integer binary search on the answer

Use when `ok(x)` is monotone: `false false ... false true true ...`. Write the invariant, not a formula you half-remember: **`lo` is always bad, `hi` is always good.**

```cpp
// smallest x in (lo, hi] with ok(x) true; requires ok(hi) true, ok(lo) false (lo may be a sentinel)
template <class F> ll firstTrue(ll lo, ll hi, F ok) {
    while (hi - lo > 1) {
        ll mid = lo + (hi - lo) / 2;
        (ok(mid) ? hi : lo) = mid;
    }
    return hi;
}
// largest good x for "true ... true false ... false": lastTrue = firstTrue of !ok, minus 1
```

- Bounds: pick `lo` = something surely infeasible (often `-1` or `0`), `hi` = surely feasible (`1e18`, `2e9`). Check that `ok` never overflows at `hi` (e.g. `mid * cnt` with `mid = 1e18` → use `__int128` or early exit).
- **Maximize the minimum** (e.g. Hockey Fans, ACPC 2024: largest D such that M non-overlapping windows of length S all have values ≥ D): `ok(D)` = greedy scan left to right taking a window as soon as the last S values are all ≥ D. Monotone because a smaller D is easier.
- **Minimize the maximum** (split array into k parts minimizing max sum; minimize max edge on a path): `ok(X)` = greedy "how many parts do I need with cap X" ≤ k.
- **k-th smallest of an implicit set** (k-th smallest pairwise sum/difference, k-th fraction): binary search value v, count elements ≤ v (two pointers on sorted arrays is usually O(n)).
- **Binary search + graph** (Arachnophobia, Horse Snatchers): fix the parameter, then run BFS/Dijkstra on the allowed subgraph and check if the target is reachable within the time limit. Complexity × log(range).
- Searching over sorted *candidate* values (e.g. distinct edge weights) instead of the whole range keeps the answer exact and iterations ≤ log n.

## Real-valued binary search

```cpp
// sketch: ok(mid) = your feasibility test
double lo = 0, hi = 1e9;
for (int it = 0; it < 100; ++it) {   // 100 halvings: 2^-100 relative; never loop on hi-lo>eps
    double mid = (lo + hi) / 2;
    if (ok(mid)) hi = mid; else lo = mid;
}
```

Fixed iteration count avoids infinite loops caused by precision. 60 iterations are enough for 1e-9 relative on [0, 1e9]; cost is usually negligible.

## Ternary search (unimodal functions)

```cpp
// maximum of strictly unimodal f on [lo, hi] (reals)
template <class F> double ternaryMax(double lo, double hi, F f) {
    for (int it = 0; it < 200; ++it) {
        double m1 = lo + (hi - lo) / 3, m2 = hi - (hi - lo) / 3;
        if (f(m1) < f(m2)) lo = m1; else hi = m2;
    }
    return (lo + hi) / 2;
}
// integers: shrink while hi - lo > 2, then check lo..hi directly
template <class F> ll ternaryMaxInt(ll lo, ll hi, F f) {
    while (hi - lo > 2) {
        ll m1 = lo + (hi - lo) / 3, m2 = hi - (hi - lo) / 3;
        if (f(m1) < f(m2)) lo = m1 + 1; else hi = m2;
    }
    ll best = lo;
    for (ll x = lo; x <= hi; ++x) if (f(x) > f(best)) best = x;
    return best;
}
```

- Integer version fails with plateaus (f(m1) == f(m2) not at the max). If f can be flat, binary search on the sign of `f(x+1) - f(x)` instead (valid when f is concave: differences are non-increasing).
- Convex functions: sums of convex functions are convex (e.g. Σ|x − a_i|, Σ dist(P(t), Q_i) for linear motion). Distance between two points moving linearly over time is convex in t → ternary search on time.
- Nested ternary search works for functions convex in 2 variables (e.g. Fermat point, smallest enclosing circle centre) but is slow: 100×100 evaluations.

## Parametric search / fractional programming

**Maximize Σa_i / Σb_i** over feasible sets S (b > 0). The answer ≥ λ iff max_S Σ(a_i − λ b_i) ≥ 0. So binary search λ and solve the *unweighted* problem with weights a_i − λb_i:

- Choose exactly k items: sort by a_i − λb_i, take top k.
- Spanning tree minimizing Σcost/Σgain (ACPC 2023 *Buildin' Roads*): min ratio ⇔ binary search λ, MST with weights cost − λ·gain, check total ≤ 0.
- Cycle with maximum mean (Σw/len): binary search λ, look for positive cycle with weights w − λ (Bellman–Ford).

**Dinkelbach** converges faster: start λ = 0; repeat: S = argmax Σ(a − λb); λ = Σ_S a / Σ_S b; until no improvement (usually < 10 iterations).

## Binary lifting search ("walk down powers of two")

When you can test "is the answer ≥ pos + 2^k" incrementally (Fenwick tree prefix walk, binary lifting on ancestors, sparse table), descend over k from high to low instead of a separate binary search — saves a log factor. See BIT `lower_bound` (§3.2) and LCA (§5.4).

## Parallel binary search (offline, many queries)

Q queries each need "first time t when condition holds" where the structure evolves over time (e.g. first time u,v connected after edge additions; first time a sum reaches a threshold). Do log T rounds: in each round sort queries by their current `mid`, replay the events once from the start, and answer all `ok(mid)` checks along the way. Cost: O((T + Q) log T × cost of one event).
