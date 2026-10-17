# Segment Tree

## Iterative (point update, range query) — fastest to type

```cpp
// [stress-tested] change T / ID / op; op must be associative (need not be commutative)
struct Seg {
    using T = ll;
    static constexpr T ID = LLONG_MIN;
    static T op(T a, T b) { return max(a, b); }
    int n; vector<T> t;
    Seg(int n) : n(n), t(2 * n, ID) {}
    void set(int i, T v) {
        for (t[i += n] = v; i > 1; i >>= 1) t[i >> 1] = op(t[i & ~1], t[i | 1]);
    }
    T query(int l, int r) {                // [l, r)
        T ra = ID, rb = ID;
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) ra = op(ra, t[l++]);
            if (r & 1) rb = op(t[--r], rb);
        }
        return op(ra, rb);
    }
};
```

## Lazy propagation: range assign + range add, query sum/min/max

```cpp
// [stress-tested] 0-indexed, inclusive [l, r]. Delete what you don't need.
struct LazySeg {
    struct Node { ll sum = 0, mn = INF, mx = -INF; };
    static Node merge(const Node& a, const Node& b) {
        return {a.sum + b.sum, min(a.mn, b.mn), max(a.mx, b.mx)};
    }
    int n; vector<Node> t; vector<ll> add, setv; vector<char> hasSet;
    LazySeg(const vector<ll>& a) : n(sz(a)), t(4 * n), add(4 * n), setv(4 * n), hasSet(4 * n) {
        build(1, 0, n - 1, a);
    }
    void build(int x, int l, int r, const vector<ll>& a) {
        if (l == r) { t[x] = {a[l], a[l], a[l]}; return; }
        int m = (l + r) / 2;
        build(2 * x, l, m, a); build(2 * x + 1, m + 1, r, a);
        t[x] = merge(t[2 * x], t[2 * x + 1]);
    }
    void applySet(int x, int l, int r, ll v) {
        t[x] = {v * (r - l + 1), v, v}; hasSet[x] = 1; setv[x] = v; add[x] = 0;
    }
    void applyAdd(int x, int l, int r, ll v) {
        t[x].sum += v * (r - l + 1); t[x].mn += v; t[x].mx += v;
        if (hasSet[x]) setv[x] += v; else add[x] += v;
    }
    void push(int x, int l, int r) {
        int m = (l + r) / 2;
        if (hasSet[x]) {
            applySet(2 * x, l, m, setv[x]); applySet(2 * x + 1, m + 1, r, setv[x]); hasSet[x] = 0;
        }
        if (add[x]) {
            applyAdd(2 * x, l, m, add[x]); applyAdd(2 * x + 1, m + 1, r, add[x]); add[x] = 0;
        }
    }
    // type 0 = add v, type 1 = assign v
    void update(int ql, int qr, ll v, int type) { update(1, 0, n - 1, ql, qr, v, type); }
    void update(int x, int l, int r, int ql, int qr, ll v, int type) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { type ? applySet(x, l, r, v) : applyAdd(x, l, r, v); return; }
        push(x, l, r);
        int m = (l + r) / 2;
        update(2 * x, l, m, ql, qr, v, type); update(2 * x + 1, m + 1, r, ql, qr, v, type);
        t[x] = merge(t[2 * x], t[2 * x + 1]);
    }
    Node query(int ql, int qr) { return query(1, 0, n - 1, ql, qr); }
    Node query(int x, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return Node();
        if (ql <= l && r <= qr) return t[x];
        push(x, l, r);
        int m = (l + r) / 2;
        return merge(query(2 * x, l, m, ql, qr), query(2 * x + 1, m + 1, r, ql, qr));
    }
    // first index i >= ql with a[i] >= v, or -1  (O(log^2 n) worst, usually ~log n)
    int firstAtLeast(int ql, ll v) { return firstAtLeast(1, 0, n - 1, ql, v); }
    int firstAtLeast(int x, int l, int r, int ql, ll v) {
        if (r < ql || t[x].mx < v) return -1;
        if (l == r) return l;
        push(x, l, r);
        int m = (l + r) / 2;
        int res = firstAtLeast(2 * x, l, m, ql, v);
        return res != -1 ? res : firstAtLeast(2 * x + 1, m + 1, r, ql, v);
    }
};
```

**How to design a new lazy segment tree:** decide (1) the node summary, (2) the tag, (3) how a tag changes a node summary *using only the summary and the segment length*, (4) how two tags compose (new tag applied after old). If (3) is impossible, the problem needs a different summary (e.g. store sum of squares to support range add with sum-of-squares query: Σ(x+v)² = Σx² + 2vΣx + len·v²).

## Useful node designs

| query | node stores | merge |
|---|---|---|
| max subarray sum | `sum, pref, suf, best` | `best = max(a.best, b.best, a.suf + b.pref)`, `pref = max(a.pref, a.sum + b.pref)` |
| count of minimum | `mn, cnt` | take smaller, add counts on tie |
| longest run of 1s | `len, pref1, suf1, best` | like max subarray |
| bracket sequence validity | `open, close` unmatched | `m = min(a.open, b.close)`; `open = a.open + b.open − m`, `close = a.close + b.close − m` |
| polynomial hash of range | `hash, len` | `a.hash · B^len_b + b.hash` |
| matrix product (DP transitions along array) | 2×2 / 3×3 matrix | matrix multiply (order matters) |
| area of union of rectangles (sweep) | `cnt, coveredLen` | no push: `covered = cnt > 0 ? len : left + right` |

## Persistent segment tree: k-th smallest in a range

```cpp
// [stress-tested] versions: root[0] = 0 (empty), root[i+1] = insert a[i] into root[i].
// kth(root[l], root[r+1], k) = k-th smallest (1-based) value index among a[l..r].
struct PST {
    struct N { int l, r, c; };
    vector<N> t; int V;                       // values are 0..V-1 (compress first)
    PST(int V) : t(1, {0, 0, 0}), V(V) {}
    int insert(int prev, int pos) { return ins(prev, 0, V - 1, pos); }
    int ins(int prev, int lo, int hi, int pos) {
        int cur = sz(t); t.push_back(t[prev]); t[cur].c++;
        if (lo < hi) {
            int mid = (lo + hi) / 2;
            if (pos <= mid) { int x = ins(t[prev].l, lo, mid, pos); t[cur].l = x; }
            else { int x = ins(t[prev].r, mid + 1, hi, pos); t[cur].r = x; }
        }
        return cur;
    }
    int kth(int a, int b, int k) {
        int lo = 0, hi = V - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2, cnt = t[t[b].l].c - t[t[a].l].c;
            if (k <= cnt) { a = t[a].l; b = t[b].l; hi = mid; }
            else { k -= cnt; a = t[a].r; b = t[b].r; lo = mid + 1; }
        }
        return lo;
    }
};
```

Memory: n·(log V + 1) nodes. The same "difference of two versions" trick counts values ≤ x in a range, online.

## Variations (when you need them)

- **Segment tree on values** (counts per value, compressed): k-th smallest by descending (go left if left count ≥ k).
- **Merge-sort tree**: each node stores the sorted subarray → count elements ≤ x in [l, r] in O(log² n). Memory n log n.
- **Dynamic / implicit segment tree**: create children on demand (`vector<array<int,2>> ch`) when coordinates are up to 10^9 and online. Offline → compress instead.
- **Persistent segment tree**: every update copies O(log n) nodes; version i = prefix i of array → k-th smallest in range via `root[r] − root[l−1]`.
- **Segment tree over time** for offline dynamic connectivity (§3.1).
- Recursion is fine for n ≤ 10^6; size arrays `4n`.
