# Sparse Table, Ordered Set, Treap, Mo's, Small-to-Large

## Sparse table (static RMQ in O(1))

Works for idempotent operations (min, max, gcd, and, or). O(n log n) build.

```cpp
// [stress-tested]
struct Sparse {
    vector<vector<ll>> t;
    static ll op(ll a, ll b) { return min(a, b); }
    Sparse(const vector<ll>& a) {
        int n = sz(a), K = __lg(max(n, 1)) + 1;
        t.assign(K, a);
        rep(k, 1, K) rep(i, 0, n - (1 << k) + 1)
            t[k][i] = op(t[k - 1][i], t[k - 1][i + (1 << (k - 1))]);
    }
    ll query(int l, int r) {             // inclusive, l <= r
        int k = __lg(r - l + 1);
        return op(t[k][l], t[k][r - (1 << k) + 1]);
    }
};
```

For non-idempotent ops (sum, product) use prefix sums / disjoint sparse table / segment tree. Range GCD with sparse table is O(log) per query due to gcd cost.

## Ordered set (PBDS)

```cpp
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace __gnu_pbds;
template <class T>
using ordered_set = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;
// os.find_by_order(k) -> iterator to k-th smallest (0-based); os.order_of_key(x) -> #elements < x
// multiset: store pair<value, unique_id>; order_of_key({x, -1}) = #elements < x
```

## Implicit treap (sequence with split/merge, reverse, range sum)

Use for: cut a block and paste elsewhere, reverse ranges, insert/delete at position — all O(log n).

```cpp
// [stress-tested]
mt19937 trng(91);
struct Treap {
    struct N { int l = 0, r = 0, sz = 0; unsigned pr = 0; ll val = 0, sum = 0; bool rev = false; };
    vector<N> t;
    Treap() { t.push_back(N()); }        // node 0 = null
    int make(ll v) { N x; x.sz = 1; x.pr = trng(); x.val = x.sum = v; t.push_back(x); return sz(t) - 1; }
    void pull(int x) {
        t[x].sz = 1 + t[t[x].l].sz + t[t[x].r].sz;
        t[x].sum = t[x].val + t[t[x].l].sum + t[t[x].r].sum;
    }
    void push(int x) {
        if (x && t[x].rev) {
            swap(t[x].l, t[x].r);
            if (t[x].l) t[t[x].l].rev ^= 1;
            if (t[x].r) t[t[x].r].rev ^= 1;
            t[x].rev = false;
        }
    }
    // first k elements -> a, rest -> b
    void split(int x, int k, int& a, int& b) {
        if (!x) { a = b = 0; return; }
        push(x);
        if (t[t[x].l].sz < k) { split(t[x].r, k - t[t[x].l].sz - 1, t[x].r, b); a = x; }
        else { split(t[x].l, k, a, t[x].l); b = x; }
        pull(x);
    }
    int merge(int a, int b) {
        if (!a || !b) return a ^ b;
        if (t[a].pr > t[b].pr) { push(a); t[a].r = merge(t[a].r, b); pull(a); return a; }
        push(b); t[b].l = merge(a, t[b].l); pull(b); return b;
    }
    // helpers on root (0-indexed, inclusive ranges)
    void insert(int& root, int pos, ll v) { int a, b; split(root, pos, a, b); root = merge(merge(a, make(v)), b); }
    void erase(int& root, int pos) { int a, b, c; split(root, pos, a, b); split(b, 1, b, c); root = merge(a, c); }
    void reverse(int& root, int l, int r) {
        int a, b, c; split(root, l, a, b); split(b, r - l + 1, b, c);
        t[b].rev ^= 1; root = merge(merge(a, b), c);
    }
    ll rangeSum(int& root, int l, int r) {
        int a, b, c; split(root, l, a, b); split(b, r - l + 1, b, c);
        ll s = t[b].sum; root = merge(merge(a, b), c); return s;
    }
    void collect(int x, vector<ll>& out) {   // in-order
        if (!x) return; push(x); collect(t[x].l, out); out.push_back(t[x].val); collect(t[x].r, out);
    }
};
```

Keyed treap (ordered by value) = same with split by key `t[x].val < k`. Add other lazy tags (range add) exactly like the segment tree: apply to node + mark children in `push`.

## Mo's algorithm (offline range queries, O((n + q)√n))

Needs O(1) (or O(log)) add/remove of one element at either end. Example: number of distinct values in [l, r].

```cpp
// [stress-tested]
vector<int> mo(const vector<int>& a, const vector<pair<int,int>>& qs) {  // inclusive queries
    int n = sz(a), q = sz(qs), B = max(1, (int)(n / sqrt(max(q, 1))));
    vector<int> ord(q); iota(all(ord), 0);
    sort(all(ord), [&](int i, int j) {
        int bi = qs[i].first / B, bj = qs[j].first / B;
        if (bi != bj) return bi < bj;
        return (bi & 1) ? qs[i].second > qs[j].second : qs[i].second < qs[j].second;
    });
    vector<int> cnt(*max_element(all(a)) + 1), res(q);   // compress a first if values are big
    int cur = 0, L = 0, R = -1;
    auto add = [&](int i) { if (cnt[a[i]]++ == 0) ++cur; };
    auto rem = [&](int i) { if (--cnt[a[i]] == 0) --cur; };
    for (int id : ord) {
        auto [l, r] = qs[id];
        while (R < r) add(++R);
        while (L > l) add(--L);
        while (R > r) rem(R--);
        while (L < l) rem(L++);
        res[id] = cur;
    }
    return res;
}
```

Expand before shrinking (as above) so the window is never "negative". Mo on trees: run on the Euler tour (ET of size 2n, LCA handled separately).

## Small-to-large merging

Merging sets of children into the parent: always insert the smaller into the larger (`if (sz(A) < sz(B)) swap(A, B);`). Each element moves O(log n) times → O(n log² n) with `set/map`, O(n log n) with hash/vectors. Classic use: for every subtree, number of distinct colours / most frequent colour / answer queries about subtree.

```cpp
// distinct colours in every subtree; g = children lists, col = colours
void dfsMerge(int v, const vector<vector<int>>& g, const vector<int>& col,
              vector<set<int>>& S, vector<int>& ans) {
    S[v].insert(col[v]);
    for (int c : g[v]) {
        dfsMerge(c, g, col, S, ans);
        if (sz(S[c]) > sz(S[v])) swap(S[c], S[v]);     // O(1) swap of std::set
        for (int x : S[c]) S[v].insert(x);
        S[c].clear();
    }
    ans[v] = sz(S[v]);
}
```

## Other standard-library power tools

- `multiset` as a priority queue with deletion; `prev(ms.end())` = max.
- `map<ll, ll>` as an "interval map" (Chtholly / ODT): store maximal constant segments `[l → value]`; range assign = split at l and r+1, erase between, insert one — amortised O(log n) with random assignments.
- Set of disjoint intervals (merge on insert): `map<l, r>`, find the first interval with `prev(upper_bound(l))`, absorb overlaps.
- `deque` supports O(1) push/pop both ends + random access.
- `bitset` §2.6; `__int128` for 128-bit keys.
