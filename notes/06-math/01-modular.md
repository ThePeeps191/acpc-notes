# Modular Arithmetic, GCD, CRT, Binomials

## Basics

```cpp
// [stress-tested]
ll power(ll b, ll e, ll m) {                      // b^e mod m, m < 2^62 safe via __int128
    ll r = 1 % m; b %= m; if (b < 0) b += m;
    for (; e > 0; e >>= 1, b = (__int128)b * b % m) if (e & 1) r = (__int128)r * b % m;
    return r;
}
ll inv(ll a, ll p) { return power(a, p - 2, p); } // p prime, a not divisible by p
ll extgcd(ll a, ll b, ll& x, ll& y) {             // a*x + b*y = g = gcd(a, b)
    if (b == 0) { x = 1; y = 0; return a; }
    ll x1, y1, g = extgcd(b, a % b, x1, y1);
    x = y1; y = x1 - (a / b) * y1;
    return g;
}
ll invGeneral(ll a, ll m) {                       // inverse mod any m, or -1 if gcd(a, m) != 1
    ll x, y; ll g = extgcd(((a % m) + m) % m, m, x, y);
    return g != 1 ? -1 : ((x % m) + m) % m;
}
```

- Always reduce after every `+`, `−` (`(a − b + MOD) % MOD`), `*` (cast to `ll` first: `1LL * a * b % MOD`).
- Division by d ⇒ multiply by `inv(d)`; valid only if gcd(d, MOD) = 1. **Don't divide before taking mod in a formula unless d divides exactly** (e.g. n(n+1)/2 mod p: compute with the inverse of 2, or divide n or n+1 first).
- **Exponent reduction** (Fermat/Euler): a^e mod p = a^(e mod (p−1)) mod p when gcd(a, p) = 1. For towers `a^(b^c) mod m` use φ(m): a^e ≡ a^(e mod φ(m) + φ(m)) for e ≥ log2 m (generalised Euler).
- Inverses of 1..n in O(n): `inv[1] = 1; inv[i] = (MOD − MOD/i) * inv[MOD % i] % MOD`.
- `mod 10^9 + 9`, `998244353`, `10^6` (not prime! no inverses) — read the statement carefully.

## Factorials and binomials

```cpp
struct Comb {                                     // needs n < p (else n! = 0 mod p)
    vector<ll> f, fi; ll p;
    Comb(int n, ll p) : f(n + 1), fi(n + 1), p(p) {
        f[0] = 1; rep(i, 1, n + 1) f[i] = f[i - 1] * i % p;
        fi[n] = power(f[n], p - 2, p);
        for (int i = n; i > 0; --i) fi[i - 1] = fi[i] * i % p;
    }
    ll C(int n, int k) const { return k < 0 || k > n ? 0 : f[n] * fi[k] % p * fi[n - k] % p; }
};
// C(n, k) for huge n (up to 1e18) and small k: product formula, p prime > k
ll Cbig(ll n, int k, ll p) {
    if (k < 0 || k > n) return 0;
    ll num = 1, den = 1;
    rep(i, 0, k) { num = (__int128)num * ((n - i) % p) % p; den = den * ((i + 1) % p) % p; }
    return num * power(den, p - 2, p) % p;
}
// Lucas: C(n, k) mod small prime p, with c = Comb(p - 1, p)
ll lucas(ll n, ll k, const Comb& c) {
    if (k < 0 || k > n) return 0;
    ll res = 1;
    while (n || k) { res = res * c.C(n % c.p, k % c.p) % c.p; n /= c.p; k /= c.p; }
    return res;
}
```

- Without a prime modulus (or n ≤ 60 exact): Pascal's triangle `C[n][k] = C[n−1][k−1] + C[n−1][k]` (fits `ll` up to n = 66 for all k; `__int128` further).
- C(n, k) mod prime power / composite: factor m, Lucas–Granville or CRT; rarely needed.

## Linear Diophantine equations and CRT

**a·x + b·y = c** solvable iff g = gcd(a, b) divides c. One solution: `x0 = x·(c/g)`, `y0 = y·(c/g)` from extgcd. All solutions: `x = x0 + k·(b/g)`, `y = y0 − k·(a/g)`. To find the smallest non-negative x: `x = ((x0 % (b/g)) + b/g) % (b/g)` (use `__int128` for the multiplication).

```cpp
// [stress-tested] x ≡ a1 (mod m1), x ≡ a2 (mod m2), moduli need not be coprime.
// Returns {x, lcm} with 0 <= x < lcm, or {-1, -1} if no solution. Keep lcm < 2^62.
pair<ll, ll> crt(ll a1, ll m1, ll a2, ll m2) {
    ll x, y, g = extgcd(m1, m2, x, y);
    if ((a2 - a1) % g != 0) return {-1, -1};
    ll l = m1 / g * m2;
    __int128 t = (__int128)(a2 - a1) / g * x % (m2 / g);
    __int128 res = (a1 + t * m1) % l;
    if (res < 0) res += l;
    return {(ll)res, l};
}
```

Fold over many congruences pairwise. Uses: combine answers mod several primes, "the clock shows x mod a and y mod b" problems, periodic events that must coincide (first time t where several cyclic processes are simultaneously in given states).

## GCD / LCM facts

- gcd(a, b) = gcd(a, b − a) = gcd(a, b mod a); gcd of an array with one changing element → prefix/suffix gcds.
- gcd(a + k, b + k) as k varies: gcd(a + k, b − a) → only divisors of |b − a| matter.
- Prefix gcds of an array take ≤ log2(max) distinct values (each change at least halves) — "all subarray gcds" has O(n log V) distinct (value, start) pairs.
- lcm can overflow: `a / gcd(a, b) * b`, check with `__int128` or cap.
- Number of pairs (i, j) ≤ n with gcd = 1 → Möbius / φ sums (§7.2). Σ_{d|n} φ(d) = n.
- `__gcd`, `std::gcd`, `std::lcm` (C++17) work on `ll`.
- **"Find p given several values s_i² mod p"** (ACPC 2024 *Four Questions*): p divides `s_i² − r_i` for each query, so p divides `gcd` of these numbers. Choose queries so the gcd is p (use s > √p so s² actually wraps; several queries remove spurious factors).

## Useful closed forms

- Σi = n(n+1)/2, Σi² = n(n+1)(2n+1)/6, Σi³ = (n(n+1)/2)², geometric Σ_{i<n} r^i = (r^n − 1)/(r − 1).
- Σ_{i=1}^{n} ⌊n/i⌋ = O(n log n) to iterate directly; computing it fast uses the O(√n) block trick (§2.5).
- Harmonic numbers H_n ≈ ln n + 0.5772. Number of primes ≤ n ≈ n / ln n (78498 below 10^6, 664579 below 10^7).
