# Disjoint Set Union

```cpp
struct DSU {
    vector<int> p;                       // p[x] < 0: x is root, size = -p[x]
    DSU(int n) : p(n, -1) {}
    int find(int x) { return p[x] < 0 ? x : p[x] = find(p[x]); }
    bool same(int a, int b) { return find(a) == find(b); }
    int size(int x) { return -p[find(x)]; }
    bool unite(int a, int b) {
        a = find(a), b = find(b);
        if (a == b) return false;
        if (p[a] > p[b]) swap(a, b);     // union by size
        p[a] += p[b]; p[b] = a;
        return true;
    }
};
```

Recursion depth is O(log n) thanks to union by size, so `find` is safe even for n = 10^6.

## Uses

- Connected components online under edge additions; Kruskal MST (§5.2); cycle detection in undirected graphs (`unite` returns false).
- **Offline deletions** (ACPC 2022 *Supply Routes*): read all operations, start from the final graph (all never-deleted edges), process operations in reverse: a deletion becomes an addition, queries are answered in reverse and output reversed.
- **"Next free slot" pointer jumping:** `nxt = DSU` where after using slot i you link i → i+1; `find(i)` gives the first free slot ≥ i. Paint-the-range problems become O(n α).
- Track per-component aggregates (sum, min, count of something) in arrays indexed by root; merge them in `unite`.
- Bipartiteness / parity constraints online: DSU with parity (below).
- Process edges in sorted order of weight to answer "max edge on path / min bottleneck" queries offline (Kruskal reconstruction tree idea: when a union happens at weight w, all pairs between the two components have bottleneck w).

## DSU with parity / potential (weighted DSU)

Maintains `d[x]` = value(x) − value(root) for relations like "x and y differ by w" or parity "x and y have different colours".

```cpp
struct WDSU {
    vector<int> p; vector<ll> d;         // d[x] = val[x] - val[parent chain root]
    WDSU(int n) : p(n, -1), d(n, 0) {}
    int find(int x) {
        if (p[x] < 0) return x;
        int r = find(p[x]);
        d[x] += d[p[x]]; p[x] = r;       // d[p[x]] is already relative to r
        return r;
    }
    // add constraint val[b] - val[a] = w; returns false on contradiction
    bool unite(int a, int b, ll w) {
        int ra = find(a), rb = find(b);
        if (ra == rb) return d[b] - d[a] == w;
        if (p[ra] > p[rb]) { swap(ra, rb); swap(a, b); w = -w; }
        // attach rb under ra: val[rb] - val[ra] = d[a] + w - d[b]
        p[ra] += p[rb]; p[rb] = ra; d[rb] = d[a] + w - d[b];
        return true;
    }
    ll diff(int a, int b) { find(a); find(b); return d[b] - d[a]; }  // assumes same set
};
```

For parity, use w ∈ {0,1} and compare `(d[b] - d[a]) & 1` (or replace + by xor).

## DSU with rollback (for offline dynamic connectivity / divide & conquer)

No path compression; `undo()` pops the last union. Each `find` is O(log n).

```cpp
struct RollbackDSU {
    vector<int> p; vector<pair<int,int>> st;   // (attached root, old parent size)
    int comps;
    RollbackDSU(int n) : p(n, -1), comps(n) {}
    int find(int x) { while (p[x] >= 0) x = p[x]; return x; }
    bool unite(int a, int b) {
        a = find(a), b = find(b);
        if (a == b) { st.push_back({-1, -1}); return false; }
        if (p[a] > p[b]) swap(a, b);
        st.push_back({b, p[b]});
        p[a] += p[b]; p[b] = a; --comps;
        return true;
    }
    void undo() {
        auto [b, sb] = st.back(); st.pop_back();
        if (b < 0) return;
        int a = p[b]; p[b] = sb; p[a] -= sb; ++comps;
    }
};
```

**Offline dynamic connectivity:** each edge is alive during a time interval; insert the interval into a segment tree over time (O(log T) nodes); DFS the segment tree, `unite` on entry, `undo` on exit, answer queries at leaves. O(m log T log n).
