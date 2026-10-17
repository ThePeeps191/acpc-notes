# Fenwick Tree (BIT)

Point update, prefix query, O(log n), tiny constant. Works for any **invertible** operation (sum, xor) for range queries; for max/min only prefix queries with non-decreasing updates.

```cpp
// [stress-tested] 0-indexed interface, internally 1-indexed
struct BIT {
    int n; vector<ll> t;
    BIT(int n) : n(n), t(n + 1, 0) {}
    void add(int i, ll v) { for (++i; i <= n; i += i & -i) t[i] += v; }
    ll pref(int i) { ll s = 0; for (; i > 0; i -= i & -i) s += t[i]; return s; }  // sum [0, i)
    ll sum(int l, int r) { return pref(r) - pref(l); }                           // sum [l, r)
    // smallest idx such that pref(idx + 1) >= k (all values must be >= 0); returns n if none
    int lower_bound(ll k) {
        int pos = 0;
        for (int pw = 1 << __lg(max(n, 1)); pw; pw >>= 1)
            if (pos + pw <= n && t[pos + pw] < k) { pos += pw; k -= t[pos]; }
        return pos;
    }
};
```

- **Order statistics:** BIT over compressed values with counts; `lower_bound(k)` = k-th smallest (1-based k). Insert/erase = `add(v, ±1)`.
- **Count inversions:** iterate from left, `ans += i - pref(a[i] + 1)` (number of previous elements > a[i]), then `add(a[i], 1)`. Values compressed to 0..n−1.
- **Count of distinct values in ranges (offline):** sort queries by r; when processing position i, `add(i, +1)` and `add(last[a[i]], −1)`; answer = sum(l, r+1).
- **2D points dominance count:** sort by x, BIT on y (sweep, §2.2).

## Range update, point query / range update, range query

```cpp
// range add, point query: store differences
struct BITRangeAdd {
    BIT b; BITRangeAdd(int n) : b(n + 1) {}
    void add(int l, int r, ll v) { b.add(l, v); b.add(r + 1, -v); }   // a[l..r] += v (inclusive)
    ll get(int i) { return b.pref(i + 1); }
};
// range add, range sum: two BITs
struct BITRangeRange {
    BIT b1, b2; BITRangeRange(int n) : b1(n + 1), b2(n + 1) {}
    void add(int l, int r, ll v) {                                    // inclusive
        b1.add(l, v); b1.add(r + 1, -v);
        b2.add(l, v * l); b2.add(r + 1, -v * (r + 1));
    }
    ll pref(int i) { return b1.pref(i) * i - b2.pref(i); }            // sum [0, i)
    ll sum(int l, int r) { return pref(r + 1) - pref(l); }            // inclusive
};
```

## 2D BIT

```cpp
struct BIT2D {   // point add, rectangle-prefix sum; n*m memory
    int n, m; vector<vector<ll>> t;
    BIT2D(int n, int m) : n(n), m(m), t(n + 1, vector<ll>(m + 1)) {}
    void add(int x, int y, ll v) {
        for (int i = x + 1; i <= n; i += i & -i)
            for (int j = y + 1; j <= m; j += j & -j) t[i][j] += v;
    }
    ll pref(int x, int y) {  // sum over [0,x) x [0,y)
        ll s = 0;
        for (int i = x; i > 0; i -= i & -i)
            for (int j = y; j > 0; j -= j & -j) s += t[i][j];
        return s;
    }
};
```

If coordinates are large and points known in advance: compress, or do the offline sweep with 1D BIT instead.
