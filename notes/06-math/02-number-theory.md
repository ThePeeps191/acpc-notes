# Primes, Factorisation, Divisors, Multiplicative Functions

## Sieves

```cpp
// [stress-tested] smallest prime factor for all n <= N (linear sieve) + list of primes. N = 1e7 ~ 0.1 s, 40 MB.
struct Sieve {
    vector<int> spf, primes;
    Sieve(int N) : spf(N + 1, 0) {
        rep(i, 2, N + 1) {
            if (!spf[i]) { spf[i] = i; primes.push_back(i); }
            for (int p : primes) {
                if (p > spf[i] || (ll)i * p > N) break;
                spf[i * p] = p;
            }
        }
    }
    vector<pair<int, int>> factor(int x) const {      // x <= N, O(log x)
        vector<pair<int, int>> f;
        while (x > 1) { int p = spf[x], e = 0; while (x % p == 0) x /= p, ++e; f.push_back({p, e}); }
        return f;
    }
};
```

- Plain Eratosthenes with `vector<bool>` handles N = 10^8 in ~1 s.
- **Segmented sieve** for primes in [L, R] with R ≤ 10^12, R − L ≤ 10^6: sieve primes up to √R, then cross out multiples in the window.
- The linear sieve also computes multiplicative functions: φ, μ, number of divisors — handle the `i % p == 0` case separately.

## Trial division (n ≤ 10^12–10^14) and factoring big numbers

```cpp
vector<pair<ll, int>> factorTrial(ll n) {      // O(sqrt n); with a primes list, O(pi(sqrt n))
    vector<pair<ll, int>> f;
    for (ll p = 2; p * p <= n; ++p) if (n % p == 0) {
        int e = 0; while (n % p == 0) n /= p, ++e; f.push_back({p, e});
    }
    if (n > 1) f.push_back({n, 1});
    return f;
}
```

"All prime factors < 10^6" (ACPC 2022 *Tricky Factoring*) ⇒ divide out primes up to 10^6 from a sieve, even if n is 10^16.

```cpp
// [stress-tested] deterministic Miller-Rabin for n < 2^64, Pollard rho. ~1 ms per 64-bit number.
ull mulmod(ull a, ull b, ull m) { return (unsigned __int128)a * b % m; }
ull powmod(ull b, ull e, ull m) { ull r = 1; b %= m; for (; e; e >>= 1, b = mulmod(b, b, m)) if (e & 1) r = mulmod(r, b, m); return r; }
bool isPrime(ull n) {
    if (n < 2) return false;
    for (ull p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) if (n % p == 0) return n == p;
    ull d = n - 1; int s = 0; while (!(d & 1)) d >>= 1, ++s;
    for (ull a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
        ull x = powmod(a % n, d, n);
        if (x == 0 || x == 1 || x == n - 1) continue;
        bool comp = true;
        rep(i, 1, s) { x = mulmod(x, x, n); if (x == n - 1) { comp = false; break; } }
        if (comp) return false;
    }
    return true;
}
ull pollard(ull n) {
    if (n % 2 == 0) return 2;
    static mt19937_64 rr(1234);
    while (true) {
        ull x = rr() % n, y = x, c = rr() % (n - 1) + 1, d = 1;
        auto f = [&](ull v) { return (mulmod(v, v, n) + c) % n; };
        while (d == 1) {
            ull q = 1, xs = x;
            rep(i, 0, 64) { x = f(x); y = f(f(y)); q = mulmod(q, x > y ? x - y : y - x, n); if (!q) break; }
            d = __gcd(q, n);
            if (d == 1) continue;
            if (d == n) {                       // batch overshot: redo step by step
                x = xs; d = 1;
                while (d == 1) { x = f(x); y = f(f(y)); d = __gcd(x > y ? x - y : y - x, n); }
            }
        }
        if (d != n) return d;
    }
}
void factorRho(ull n, map<ull, int>& f) {
    if (n == 1) return;
    if (isPrime(n)) { f[n]++; return; }
    ull d = pollard(n); factorRho(d, f); factorRho(n / d, f);
}
```

## Divisors

- Enumerate all divisors from the factorisation (recursive product over prime powers). Count = Π(e_i + 1), sum σ = Π (p^(e+1) − 1)/(p − 1).
- Max divisor count: ≤ 10^6 → 240; ≤ 10^9 → 1344; ≤ 10^12 → 6720; ≤ 10^16 → 41472; ≤ 10^18 → 103680. So "iterate over all divisors" or even "all pairs of divisors" of a single number is often fine.
- **Harmonic enumeration:** for every d ≤ N, iterate multiples of d: Σ N/d = O(N log N). Use for "count pairs with gcd = d" (cnt[d] = #multiples choose 2, then subtract cnt of multiples of d from larger to smaller).
- Sum of proper divisors (aliquot, ACPC 2023 *Outlaw Numbers*) = σ(n) − n.
- Counting divisors of all numbers ≤ N: `for d: for m = d; m <= N; m += d: cnt[m]++`.

## Euler's totient and Möbius

```cpp
ll phi(ll n) {                    // O(sqrt n)
    ll r = n;
    for (ll p = 2; p * p <= n; ++p) if (n % p == 0) { while (n % p == 0) n /= p; r -= r / p; }
    if (n > 1) r -= r / n;
    return r;
}
// sieve: phi[i] = i; for each prime p: for multiples m: phi[m] -= phi[m] / p
```

- φ(n) = #{1 ≤ k ≤ n : gcd(k, n) = 1}; Σ_{d|n} φ(d) = n; a^φ(m) ≡ 1 (mod m) when gcd(a, m) = 1.
- **Möbius** μ(n) = 0 if a square divides n, else (−1)^(#primes). Σ_{d|n} μ(d) = [n = 1].
- **Möbius inversion:** if g(n) = Σ_{d|n} f(d) then f(n) = Σ_{d|n} μ(d) g(n/d). Same for "multiples": if G(d) = Σ_{d|m} F(m) then F(d) = Σ_{d|m} μ(m/d) G(m).
- **Count coprime pairs** (i ≤ A, j ≤ B): Σ_d μ(d)·⌊A/d⌋·⌊B/d⌋ — with the √ block trick O(√n) per query.
- Σ_{i=1}^{n} gcd(i, n) = Σ_{d|n} d·φ(n/d).

## Other facts

- **Legendre:** exponent of prime p in n! = Σ ⌊n/p^k⌋. Trailing zeros of n! = exponent of 5.
- **Kummer:** exponent of p in C(n, k) = number of carries when adding k and n − k in base p. C(n, k) odd ⇔ (k & n) == k.
- **Wilson:** (p − 1)! ≡ −1 (mod p). **Fermat:** a^(p−1) ≡ 1.
- Every integer > 1 is a product of primes; there are infinitely many primes in every coprime residue class; prime gaps below 2^64 are ≤ 1550 (so "next prime after n" by testing successive numbers is fast).
- **Goldbach** (verified far beyond contest ranges): even n > 2 is a sum of two primes; odd n > 5 is a sum of three primes.
- **Quadratic residues:** for odd prime p, exactly (p−1)/2 non-zero squares; Euler's criterion a^((p−1)/2) ≡ ±1.
- Integer `a^b` exact comparison without overflow: compare `b·log(a)` with a margin, then verify exactly with `__int128`/capped multiplication (ACPC 2021 *Associative Exponents*).
