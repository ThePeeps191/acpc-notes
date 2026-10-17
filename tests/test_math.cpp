// uses: 06-math/01-modular.md power extgcd invGeneral Comb Cbig lucas crt
// uses: 06-math/02-number-theory.md Sieve factorTrial mulmod isPrime pollard factorRho phi
// uses: 06-math/04-linear-algebra.md mul matpow berlekampMassey linearRec gauss XorBasis
// uses: 06-math/05-fft.md fft multiply npow ntt multiplyMod
mt19937_64 rg(31337);
ll R(ll a, ll b) { return uniform_int_distribution<ll>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
int main() {
    int it = 0;
    Sieve sv(100000);
    CHECK(sz(sv.primes) == 9592);
    // primality vs sieve, factorization
    for (it = 2; it < 100000; ++it) { CHECK(isPrime(it) == (sv.spf[it] == it)); }
    for (ull p : {998244353ULL, 1000000007ULL, 1000000000000000003ULL, 4611686018427387847ULL}) { it = 0; CHECK(isPrime(p)); }
    CHECK(!isPrime(1000000007ULL * 998244353ULL) && !isPrime(3215031751ULL) && !isPrime(561));
    for (it = 0; it < 300; ++it) {
        ull a = R(2, 2e9), b = R(2, 2e9); ull n = a * b; map<ull, int> f; factorRho(n, f);
        ull prod = 1; for (auto [p, e] : f) { CHECK(isPrime(p)); rep(i, 0, e) prod *= p; } CHECK(prod == n);
        ll x = R(1, 1e6); auto ft = factorTrial(x); ll pr = 1; for (auto [p, e] : ft) rep(i, 0, e) pr *= p; CHECK(pr == x);
        auto fs = sv.factor(R(1, 100000)); for (auto [p, e] : fs) CHECK(sv.spf[p] == p);
        ll y = R(1, 5000); ll cnt = 0; rep(k, 1, y + 1) cnt += __gcd((ll)k, y) == 1; CHECK(phi(y) == cnt);
    }
    for (it = 0; it < 2000; ++it) {
        ll m = R(1, 1e9), b = R(-1e9, 1e9), e = R(0, 50); ll r = 1 % m, bb = ((b % m) + m) % m; rep(i, 0, e) r = (__int128)r * bb % m;
        CHECK(power(b, e, m) == r);
        ll a = R(1, 1e6), mm = R(1, 1e6); ll iv = invGeneral(a, mm);
        if (__gcd(a, mm) == 1) { CHECK(iv >= 0 && (__int128)a * iv % mm == 1 % mm); } else { CHECK(iv == -1); }
        // crt vs brute
        ll m1 = R(1, 30), m2 = R(1, 30), a1 = R(0, m1 - 1), a2 = R(0, m2 - 1);
        auto [x, l] = crt(a1, m1, a2, m2); ll bx = -1; rep(t, 0, m1 * m2) if (t % m1 == a1 && t % m2 == a2) { bx = t; break; }
        CHECK(x == bx);
        if (x >= 0) { CHECK(l == m1 / __gcd(m1, m2) * m2); }
    }
    // binomials
    { it = 0; Comb c(1000, MOD); ll pas[60][60] = {}; rep(i, 0, 60) { pas[i][0] = 1; rep(j, 1, i + 1) pas[i][j] = pas[i - 1][j - 1] + pas[i - 1][j]; }
      rep(i, 0, 60) rep(j, 0, i + 1) { CHECK(c.C(i, j) == pas[i][j] % MOD); CHECK(Cbig(i, j, MOD) == pas[i][j] % MOD); }
      Comb c7(6, 7); rep(i, 0, 60) rep(j, 0, i + 1) CHECK(lucas(i, j, c7) == pas[i][j] % 7); }
    // matrix power: fibonacci; BM + linearRec
    { it = 0; Mat A = {{1, 1}, {1, 0}}; vector<ll> fib(200); fib[0] = 0; fib[1] = 1; rep(i, 2, 200) fib[i] = (fib[i - 1] + fib[i - 2]) % MOD;
      rep(n, 1, 200) CHECK(matpow(A, n, MOD)[0][1] == fib[n]);
      for (it = 0; it < 200; ++it) {
          int k = R(1, 5); vector<ll> c(k), s(k); for (auto& x : c) x = R(0, MOD - 1); for (auto& x : s) x = R(0, MOD - 1);
          vector<ll> seq = s; rep(i, k, 60) { ll v = 0; rep(j, 0, k) v = (v + c[j] * seq[i - 1 - j]) % MOD; seq.push_back(v); }
          auto tr = berlekampMassey(vector<ll>(seq.begin(), seq.begin() + 2 * k + 2), MOD);
          rep(q, 0, 60) CHECK(linearRec(seq, tr, q, MOD) == seq[q]);
      } }
    // gauss
    for (it = 0; it < 300; ++it) {
        int n = R(1, 5); vector<vector<double>> a(n, vector<double>(n + 1)); vector<double> xs(n);
        for (auto& v : xs) v = R(-10, 10);
        rep(i, 0, n) { double s = 0; rep(j, 0, n) { a[i][j] = R(-10, 10); s += a[i][j] * xs[j]; } a[i][n] = s; }
        vector<double> ans; int res = gauss(a, ans); CHECK(res != 0);
        rep(i, 0, n) { double s = 0; rep(j, 0, n) s += a[i][j] * ans[j]; CHECK(fabs(s - a[i][n]) < 1e-6); }
    }
    // xor basis vs brute
    for (it = 0; it < 300; ++it) {
        int n = R(0, 8); XorBasis xb; set<ll> reach = {0};
        rep(i, 0, n) { ll v = R(0, 63); xb.insert(v); set<ll> nr = reach; for (ll r : reach) nr.insert(r ^ v); reach = nr; }
        CHECK((1LL << xb.cnt) == sz(reach)); CHECK(xb.maxXor() == *reach.rbegin());
        rep(v, 0, 64) CHECK(xb.canMake(v) == (bool)reach.count(v));
    }
    // fft / ntt vs naive
    for (it = 0; it < 300; ++it) {
        int n = R(1, 60), m = R(1, 60); vector<ll> a(n), b(m); for (auto& x : a) x = R(0, 1e6); for (auto& x : b) x = R(0, 1e6);
        vector<ll> naive(n + m - 1, 0), nm(n + m - 1, 0); rep(i, 0, n) rep(j, 0, m) { naive[i + j] += a[i] * b[j]; nm[i + j] = (nm[i + j] + a[i] * b[j]) % NMOD; }
        CHECK(multiply(a, b) == naive); CHECK(multiplyMod(a, b) == nm);
    }
    { it = 0; vector<ll> a(300000), b(300000); for (auto& x : a) x = R(0, 1000); for (auto& x : b) x = R(0, 1000);
      auto c = multiply(a, b); rep(q, 0, 20) { int k = R(0, 599998); ll s = 0; rep(i, max(0, k - 299999), min(k, 299999) + 1) s += a[i] * b[k - i]; CHECK(c[k] == s); } }
    puts("math ok");
}
