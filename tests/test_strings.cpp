// uses: 07-strings/01-hashing-kmp-z.md Hasher prefixFunction kmpFind zFunction
// uses: 07-strings/02-palindromes-rotations-sa.md manacher minRotation Trie AhoCorasick SuffixArray SAM
mt19937 rg(4242);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
string rs(int n, int k) { string s; rep(i, 0, n) s += char('a' + R(0, k - 1)); return s; }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
int main() {
    for (int it = 0; it < 3000; ++it) {
        int n = R(1, 14), k = R(1, 3); string s = rs(n, k);
        Hasher H(s);
        rep(q, 0, 20) { int l1 = R(0, n - 1), l2 = R(0, n - 1), len = R(0, n - max(l1, l2));
            CHECK((H.get(l1, l1 + len) == H.get(l2, l2 + len)) == (s.substr(l1, len) == s.substr(l2, len))); }
        auto pi = prefixFunction(s);
        rep(i, 0, n) { int b = 0; rep(L, 1, i + 1) if (s.substr(0, L) == s.substr(i + 1 - L, L)) b = L; CHECK(pi[i] == b); }
        auto z = zFunction(s);
        rep(i, 0, n) { int L = 0; while (i + L < n && s[L] == s[i + L]) ++L; CHECK(z[i] == L); }
        string pat = rs(R(1, 3), k); auto occ = kmpFind(s, pat); vector<int> bo;
        rep(i, 0, n - sz(pat) + 1) if (s.substr(i, sz(pat)) == pat) bo.push_back(i);
        CHECK(occ == bo);
        // manacher
        auto p = manacher(s); int best = 0, cntPal = 0;
        rep(l, 0, n) rep(r, l, n) { string t = s.substr(l, r - l + 1), u = t; reverse(all(u)); if (t == u) { best = max(best, r - l + 1); cntPal++;
            CHECK(p[l + r + 1] - 1 >= r - l + 1); } else CHECK(p[l + r + 1] - 1 < r - l + 1); }
        int mb = 0, cp = 0; for (int x : p) mb = max(mb, x - 1), cp += x / 2; CHECK(mb == best && cp == cntPal);
        // min rotation
        int mr = minRotation(s); string bestRot = s; rep(i, 0, n) bestRot = min(bestRot, s.substr(i) + s.substr(0, i));
        CHECK(s.substr(mr) + s.substr(0, mr) == bestRot);
        // trie + aho
        Trie tr; AhoCorasick ac; vector<string> pats; rep(i, 0, R(1, 5)) { pats.push_back(rs(R(1, 3), k)); tr.insert(pats.back()); ac.insert(pats.back()); }
        ac.build(); ll bm = 0; for (auto& q : pats) rep(i, 0, n - sz(q) + 1) bm += s.substr(i, sz(q)) == q;
        CHECK(ac.countMatches(s) == bm);
        string pre = rs(R(0, 2), k); int bc = 0; for (auto& q : pats) bc += q.substr(0, sz(pre)) == pre && sz(q) >= sz(pre); CHECK(tr.countPrefix(pre) == bc);
        // suffix array
        SuffixArray sa(s); vector<int> idx(n); iota(all(idx), 0); sort(all(idx), [&](int a, int b) { return s.substr(a) < s.substr(b); });
        CHECK(sa.sa[0] == n); rep(i, 0, n) CHECK(sa.sa[i + 1] == idx[i]);
        rep(i, 2, n + 1) { int a = sa.sa[i - 1], b = sa.sa[i], L = 0; while (a + L < n && b + L < n && s[a + L] == s[b + L]) ++L; CHECK(sa.lcp[i] == L); }
        // SAM distinct substrings
        SAM sam; for (char c : s) sam.extend(c); ll ds = 0; rep(v, 1, sz(sam.st)) ds += sam.st[v].len - sam.st[sam.st[v].link].len;
        set<string> all_; rep(l, 0, n) rep(r, l, n) all_.insert(s.substr(l, r - l + 1)); CHECK(ds == sz(all_));
        ll lcpSum = 0; rep(i, 1, n + 1) lcpSum += sa.lcp[i]; CHECK((ll)n * (n + 1) / 2 - lcpSum == sz(all_));
    }
    puts("strings ok");
}
