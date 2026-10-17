# Matrices, Linear Recurrences, Gaussian Elimination, XOR Basis

## Matrix exponentiation

```cpp
// [stress-tested]
using Mat = vector<vector<ll>>;
Mat mul(const Mat& A, const Mat& B, ll mod) {
    int n = sz(A), m = sz(B[0]), k = sz(B);
    Mat C(n, vector<ll>(m, 0));
    rep(i, 0, n) rep(t, 0, k) if (A[i][t]) rep(j, 0, m) C[i][j] = (C[i][j] + A[i][t] * B[t][j]) % mod;
    return C;
}
Mat matpow(Mat A, ll e, ll mod) {
    int n = sz(A); Mat R(n, vector<ll>(n, 0)); rep(i, 0, n) R[i][i] = 1 % mod;
    for (; e > 0; e >>= 1, A = mul(A, A, mod)) if (e & 1) R = mul(R, A, mod);
    return R;
}
```

- **Linear recurrence** f(n) = c_1 f(n−1) + … + c_k f(n−k): companion matrix (first row = c, sub-diagonal ones), O(k³ log n). Add a constant/polynomial term by adding extra state rows (e.g. a row that stays 1).
- **Number of walks of length L** between all pairs = A^L (adjacency matrix). Shortest walk with exactly L edges: (min, +) matrix "multiplication" with the same `matpow` skeleton.
- **DP with a fixed linear transition repeated 10^18 times** (e.g. tiling a 4 × n strip, states = profiles) → matrix power over the state vector.
- Cost is k³·log n: k = 100 → 10^6·60 = fine; k = 200 is the practical limit.
- Fibonacci: [[1,1],[1,0]]^n = [[F_{n+1}, F_n],[F_n, F_{n−1}]]. Identities: F_{2n} = F_n(2F_{n+1} − F_n), F_{2n+1} = F_n² + F_{n+1}².

## Berlekamp–Massey: guess the recurrence from terms

Compute the first ~2k terms of any sequence that satisfies a linear recurrence of order ≤ k (by brute force / DP), feed them to BM, get the recurrence; then jump to the n-th term in O(k² log n). Great for "count tilings/paths of size n ≤ 10^18" when the DP is linear but you don't want to derive the matrix.

```cpp
// [stress-tested] mod must be prime. Returns tr with s[i] = sum_j tr[j] * s[i-1-j].
vector<ll> berlekampMassey(const vector<ll>& s, ll mod) {
    auto pw = [&](ll x, ll e) { ll r = 1; for (x %= mod; e; e >>= 1, x = x * x % mod) if (e & 1) r = r * x % mod; return r; };
    int n = sz(s), L = 0, m = 0; vector<ll> C(n, 0), B(n, 0), T; C[0] = B[0] = 1; ll b = 1;
    rep(i, 0, n) {
        ++m; ll d = s[i] % mod;
        rep(j, 1, L + 1) d = (d + C[j] * s[i - j]) % mod;
        if (!d) continue;
        T = C; ll coef = d * pw(b, mod - 2) % mod;
        rep(j, m, n) C[j] = ((C[j] - coef * B[j - m]) % mod + mod) % mod;
        if (2 * L > i) continue;
        L = i + 1 - L; B = T; b = d; m = 0;
    }
    C.resize(L + 1); C.erase(C.begin());
    for (ll& x : C) x = (mod - x) % mod;
    return C;
}
// k-th term (0-indexed) given first terms S (size >= sz(tr)) and recurrence tr
ll linearRec(const vector<ll>& S, const vector<ll>& tr, ll k, ll mod) {
    int n = sz(tr);
    if (n == 0) return 0;
    auto combine = [&](const vector<ll>& a, const vector<ll>& b) {
        vector<ll> res(2 * n + 1, 0);
        rep(i, 0, n + 1) rep(j, 0, n + 1) res[i + j] = (res[i + j] + a[i] * b[j]) % mod;
        for (int i = 2 * n; i > n; --i) rep(j, 0, n) res[i - 1 - j] = (res[i - 1 - j] + res[i] * tr[j]) % mod;
        res.resize(n + 1); return res;
    };
    vector<ll> pol(n + 1, 0), e(n + 1, 0); pol[0] = e[1] = 1;
    for (++k; k; k /= 2) { if (k % 2) pol = combine(pol, e); e = combine(e, e); }
    ll res = 0;
    rep(i, 0, n) res = (res + pol[i + 1] * S[i]) % mod;
    return res;
}
```

Give BM at least 2·(order) terms, preferably more (it returns the shortest recurrence consistent with what it saw).

## Gaussian elimination (reals)

```cpp
// [stress-tested] a = n rows of (m coefficients + rhs). Returns 0: no solution, 1: unique, 2: infinitely many.
// ans = one solution (free variables = 0).
int gauss(vector<vector<double>> a, vector<double>& ans) {
    const double EPS = 1e-9;
    int n = sz(a), m = sz(a[0]) - 1; vector<int> where(m, -1);
    for (int col = 0, row = 0; col < m && row < n; ++col) {
        int sel = row;
        rep(i, row, n) if (fabs(a[i][col]) > fabs(a[sel][col])) sel = i;
        if (fabs(a[sel][col]) < EPS) continue;
        swap(a[sel], a[row]); where[col] = row;
        rep(i, 0, n) if (i != row) {
            double c = a[i][col] / a[row][col];
            rep(j, col, m + 1) a[i][j] -= a[row][j] * c;
        }
        ++row;
    }
    ans.assign(m, 0);
    rep(i, 0, m) if (where[i] != -1) ans[i] = a[where[i]][m] / a[where[i]][i];
    rep(i, 0, n) {
        double s = 0; rep(j, 0, m) s += ans[j] * a[i][j];
        if (fabs(s - a[i][m]) > EPS) return 0;
    }
    rep(i, 0, m) if (where[i] == -1) return 2;
    return 1;
}
```

- **Expected values with cycles** (random walk on a graph, Markov chains): one equation per state `E_v − Σ p_vu E_u = c_v`, absorbing states `E = 0`. n ≤ 500.
- Mod p version: same code with modular inverse instead of division (exact, no EPS). **Determinant** = product of pivots × (−1)^(#swaps); **rank** = number of pivots.
- **Kirchhoff (matrix-tree):** #spanning trees = any cofactor of the Laplacian L = D − A (delete one row and column, take determinant; mod p or doubles for small n).

## XOR basis (Gaussian elimination over GF(2))

```cpp
// [stress-tested]
struct XorBasis {
    ll b[62] = {}; int cnt = 0;
    bool insert(ll x) {                              // returns false if x already representable
        for (int i = 61; i >= 0; --i) if (x >> i & 1) {
            if (!b[i]) { b[i] = x; ++cnt; return true; }
            x ^= b[i];
        }
        return false;
    }
    bool canMake(ll x) const { for (int i = 61; i >= 0; --i) if (x >> i & 1) x ^= b[i]; return x == 0; }
    ll maxXor(ll x = 0) const { for (int i = 61; i >= 0; --i) x = max(x, x ^ b[i]); return x; }
};
```

- Number of distinct subset-xors = 2^cnt; each achievable value is hit by 2^(n − cnt) subsets.
- **Max xor path** in an undirected graph between u and v: xor of any path + any subset of cycle xors → DFS xor-to-root `d`, insert `d[a] ^ d[b] ^ w` for every non-tree edge, answer `maxXor(d[u] ^ d[v])`.
- Systems of equations mod 2 with up to ~2000 variables: use `bitset<N>` rows, elimination O(n³/64).
- Lights-out style puzzles (press toggles neighbours): linear system over GF(2).
