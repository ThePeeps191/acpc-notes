# FFT / NTT (Convolution)

Convolution c_k = Σ_{i+j=k} a_i b_j in O(n log n). Recognise it: **"for every shift/offset/sum k, aggregate over pairs (i, j) with i + j = k (or i − j = k)"**. Reverse one array to turn differences into sums.

## FFT with doubles (exact for integer results up to ~10^14)

```cpp
// [stress-tested]
using cd = complex<double>;
void fft(vector<cd>& a, bool invert) {
    int n = sz(a);
    for (int i = 1, j = 0; i < n; ++i) {                 // bit reversal
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
    vector<cd> rt(max(1, n / 2));                         // accurate roots, computed in long double
    rep(k, 0, n / 2) { long double ang = 2 * acosl(-1) * k / n; rt[k] = cd(cosl(ang), sinl(ang)); }
    for (int len = 2; len <= n; len <<= 1) {
        int step = n / len;
        for (int i = 0; i < n; i += len) rep(j, 0, len / 2) {
            cd w = rt[j * step]; if (invert) w = conj(w);
            cd u = a[i + j], v = a[i + j + len / 2] * w;
            a[i + j] = u + v; a[i + j + len / 2] = u - v;
        }
    }
    if (invert) for (cd& x : a) x /= n;
}
vector<ll> multiply(const vector<ll>& a, const vector<ll>& b) {
    if (a.empty() || b.empty()) return {};
    int n = 1; while (n < sz(a) + sz(b)) n <<= 1;
    vector<cd> fa(all(a)), fb(all(b)); fa.resize(n); fb.resize(n);
    fft(fa, false); fft(fb, false);
    rep(i, 0, n) fa[i] *= fb[i];
    fft(fa, true);
    vector<ll> res(sz(a) + sz(b) - 1);
    rep(i, 0, sz(res)) res[i] = llround(fa[i].real());
    return res;
}
```

## NTT modulo 998244353 (exact)

```cpp
// [stress-tested] mod = 998244353 = 119 * 2^23 + 1, primitive root 3. Max length 2^23.
const ll NMOD = 998244353;
ll npow(ll b, ll e) {
    ll r = 1; b %= NMOD;
    for (; e; e >>= 1, b = b * b % NMOD) if (e & 1) r = r * b % NMOD;
    return r;
}
void ntt(vector<ll>& a, bool invert) {
    int n = sz(a);
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        ll w = npow(3, (NMOD - 1) / len); if (invert) w = npow(w, NMOD - 2);
        for (int i = 0; i < n; i += len) {
            ll wn = 1;
            rep(j, 0, len / 2) {
                ll u = a[i + j], v = a[i + j + len / 2] * wn % NMOD;
                a[i + j] = u + v < NMOD ? u + v : u + v - NMOD;
                a[i + j + len / 2] = u - v >= 0 ? u - v : u - v + NMOD;
                wn = wn * w % NMOD;
            }
        }
    }
    if (invert) { ll ni = npow(n, NMOD - 2); for (ll& x : a) x = x * ni % NMOD; }
}
vector<ll> multiplyMod(vector<ll> a, vector<ll> b) {
    if (a.empty() || b.empty()) return {};
    int need = sz(a) + sz(b) - 1, n = 1; while (n < need) n <<= 1;
    a.resize(n); b.resize(n); ntt(a, false); ntt(b, false);
    rep(i, 0, n) a[i] = a[i] * b[i] % NMOD;
    ntt(a, true); a.resize(need); return a;
}
```

For mod 10^9+7 (not NTT-friendly): split each coefficient as `x = hi·2^15 + lo` and do 3–4 double FFTs, or three NTT primes + CRT. Usually the problem uses 998244353 if it intends NTT.

## Applications

- **Count pairs by sum:** `cnt = multiply(freq, freq)`; pairs with i < j: subtract the diagonal (`freq[x]` at index 2x) and halve.
- **Pattern matching with mismatches / count matching positions** (for every alignment i, how many j have text[i+j] == pat[j]): for each letter c, `multiply(isC(text), reverse(isC(pat)))`; entry at index `i + m − 1` counts matches for alignment i. Sum over letters (or over the 2 colours of a binary image). O(σ·n log n).
- **Wildcards** (`?` matches anything): with p_j = 0 for `?`, alignment i matches iff Σ_j p_j t_{i+j}(p_j − t_{i+j})² = 0 → expands to 3 convolutions. Use random values for letters to avoid accidental zeros when working mod p.
- **2-D matching** (ACPC 2017 *Matchings*: count equal pixels for every placement of a small image inside a big one): flatten both images with the **big image's width W**: index r·W + c. Place the small image's pixel (r, c) at r·W + c too. Then one 1-D correlation gives all placements; only use top-left positions with c + w ≤ W and r + h ≤ H.
- **Big integer multiplication:** digits as coefficients, then carry.
- **Subset sums with multiplicities / knapsack counting** with many items of small weight: multiply generating polynomials (divide & conquer on the product to keep sizes balanced).
- **Polynomial with sums over all pairs of differences** |a_i − a_j| = d for every d: multiply freq by reversed freq.
- Sizes: n up to ~10^6 per multiplication is fine (a few hundred ms).
