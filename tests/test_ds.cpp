// uses: 02-data-structures/02-fenwick.md BIT BITRangeRange
// uses: 02-data-structures/03-segment-tree.md Seg LazySeg
// uses: 02-data-structures/01-dsu.md DSU WDSU RollbackDSU
mt19937 rg(12345);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }

int main() {
    for (int it = 0; it < 3000; ++it) {
        int n = R(1, 30);
        vector<ll> a(n);
        for (auto& x : a) x = R(-20, 20);
        BIT bit(n); BITRangeRange br(n); Seg sg(n); LazySeg ls(a);
        vector<ll> pos(n);  // non-negative array for BIT lower_bound
        rep(i, 0, n) { br.add(i, i, a[i]); sg.set(i, a[i]); }
        vector<ll> b = a;
        for (int q = 0; q < 60; ++q) {
            int l = R(0, n - 1), r = R(l, n - 1), t = R(0, 5);
            ll v = R(-10, 10);
            if (t == 0) {  // range add
                ls.update(l, r, v, 0); br.add(l, r, v);
                rep(i, l, r + 1) b[i] += v;
                rep(i, l, r + 1) sg.set(i, b[i]);
            } else if (t == 1) {  // range assign
                ls.update(l, r, v, 1);
                rep(i, l, r + 1) { br.add(i, i, v - b[i]); b[i] = v; sg.set(i, v); }
            } else if (t == 2) {
                auto nd = ls.query(l, r);
                ll s = 0, mn = LLONG_MAX, mx = LLONG_MIN;
                rep(i, l, r + 1) s += b[i], mn = min(mn, b[i]), mx = max(mx, b[i]);
                CHECK(nd.sum == s && nd.mn == mn && nd.mx == mx);
                CHECK(br.sum(l, r) == s);
                CHECK(sg.query(l, r + 1) == mx);
            } else if (t == 3) {
                int got = ls.firstAtLeast(l, v), exp = -1;
                rep(i, l, n) if (b[i] >= v) { exp = i; break; }
                CHECK(got == exp);
            } else if (t == 4) {  // BIT on nonneg array + lower_bound
                int i = R(0, n - 1); ll w = R(0, 5);
                bit.add(i, w); pos[i] += w;
                ll k = R(1, 40); ll acc = 0; int exp = n;
                rep(j, 0, n) { acc += pos[j]; if (acc >= k) { exp = j; break; } }
                CHECK(bit.lower_bound(k) == exp);
                CHECK(bit.sum(l, r + 1) == accumulate(pos.begin() + l, pos.begin() + r + 1, 0LL));
            }
        }
        // DSU family vs brute force labels
        DSU d(n); WDSU w(n); RollbackDSU rb(n);
        vector<int> lab(n); iota(all(lab), 0);
        vector<ll> val(n); for (auto& x : val) x = R(-50, 50);
        for (int q = 0; q < 40; ++q) {
            int x = R(0, n - 1), y = R(0, n - 1);
            bool sameB = lab[x] == lab[y];
            CHECK(d.same(x, y) == sameB);
            CHECK(rb.find(x) == rb.find(y) == sameB);
            if (sameB) CHECK(w.diff(x, y) == val[y] - val[x]);
            CHECK(w.unite(x, y, val[y] - val[x]));
            d.unite(x, y); rb.unite(x, y);
            int o = lab[y]; for (auto& L : lab) if (L == o) L = lab[x];
        }
        int c = 0; rep(i, 0, n) c += lab[i] == i;
        CHECK(rb.comps == c);
        // rollback everything
        while (!rb.st.empty()) rb.undo();
        CHECK(rb.comps == n);
    }
    puts("ds ok");
}
