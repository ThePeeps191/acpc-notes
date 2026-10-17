// uses: 02-data-structures/04-more-structures.md Treap Sparse mo
mt19937 rg(7);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
int main() {
    for (int it = 0; it < 2000; ++it) {
        Treap T; int root = 0; vector<ll> b;
        for (int q = 0; q < 80; ++q) {
            int t = R(0, 3);
            if (t == 0 || b.empty()) { int p = R(0, sz(b)); ll v = R(-9, 9); T.insert(root, p, v); b.insert(b.begin() + p, v); }
            else if (t == 1) { int p = R(0, sz(b) - 1); T.erase(root, p); b.erase(b.begin() + p); }
            else if (t == 2) { int l = R(0, sz(b) - 1), r = R(l, sz(b) - 1); T.reverse(root, l, r); reverse(b.begin() + l, b.begin() + r + 1); }
            else { int l = R(0, sz(b) - 1), r = R(l, sz(b) - 1); CHECK(T.rangeSum(root, l, r) == accumulate(b.begin() + l, b.begin() + r + 1, 0LL)); }
            vector<ll> out; T.collect(root, out); CHECK(out == b);
        }
        int n = R(1, 40); vector<ll> a(n); for (auto& x : a) x = R(-50, 50);
        Sparse sp(a);
        rep(q, 0, 30) { int l = R(0, n - 1), r = R(l, n - 1); CHECK(sp.query(l, r) == *min_element(a.begin() + l, a.begin() + r + 1)); }
        vector<int> c(n); for (auto& x : c) x = R(0, 6);
        vector<pair<int,int>> qs; rep(q, 0, 25) { int l = R(0, n - 1), r = R(l, n - 1); qs.push_back({l, r}); }
        auto res = mo(c, qs);
        rep(q, 0, 25) { set<int> s(c.begin() + qs[q].first, c.begin() + qs[q].second + 1); CHECK(res[q] == sz(s)); }
    }
    puts("treap/sparse/mo ok");
}
