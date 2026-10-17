# Template, Debugging, Stress Testing

## Main template

```cpp
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
#define rep(i, a, b) for (int i = (a); i < (b); ++i)
#ifdef LOCAL
#define dbg(...) cerr << "[" << #__VA_ARGS__ << "] = ", _dbg(__VA_ARGS__)
template <class T> void _dbg(const T& x) { cerr << x << endl; }
template <class T, class... R> void _dbg(const T& x, const R&... r) {
    cerr << x << ", "; _dbg(r...);
}
#else
#define dbg(...)
#endif
const ll INF = 4e18;  // careful: INF + INF overflows. Use 1e18 if you add two.
const int MOD = 1e9 + 7;

void solve() {
}

int main() {
    cin.tie(nullptr)->sync_with_stdio(false);
    int T = 1;
    // cin >> T;
    while (T--) solve();
}
```

Compile locally: `g++ -std=gnu++17 -O2 -Wall -Wextra -DLOCAL -fsanitize=address,undefined -g a.cpp`.
Sanitizers catch out-of-bounds and signed overflow immediately — use them whenever you get RE/WA.

## Useful snippets

```cpp
// print __int128
void print128(__int128 x) {
    if (x < 0) { putchar('-'); x = -x; }
    if (x > 9) print128(x / 10);
    putchar('0' + (int)(x % 10));
}
// floor/ceil division that work for negatives
ll fdiv(ll a, ll b) { return a / b - ((a % b != 0) && ((a < 0) != (b < 0))); }
ll cdiv(ll a, ll b) { return a / b + ((a % b != 0) && ((a < 0) == (b < 0))); }
// exact integer sqrt (floor)
ll isqrt(ll n) { ll r = sqrtl((ld)n); while (r * r > n) --r; while ((r + 1) * (r + 1) <= n) ++r; return r; }
// random
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
ll rnd(ll l, ll r) { return uniform_int_distribution<ll>(l, r)(rng); }
// anti-hack hash for unordered_map<ll, V, chash>
struct chash {
    static ull sm(ull x) { x += 0x9e3779b97f4a7c15; x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb; return x ^ (x >> 31); }
    size_t operator()(ull x) const { static const ull R = rng(); return sm(x + R); }
};
// hash a pair: use key = (ll)a << 32 | (unsigned)b
```

- Read until EOF: `while (cin >> x)`. Read a line: `getline(cin, s)`; after `cin >> n` call `cin.ignore()` (or `getline` once) before the first `getline`.
- Output many lines: use `'\n'`, never `endl` (except interactive).
- Doubles: `cout << fixed << setprecision(10) << x;` — avoid printing `-0.000000` (add `+ 0.0` or check `abs(x) < eps`).
- Sorting with comparator must be a *strict weak ordering* (`<`, never `<=`), otherwise RE/UB.
- `lower_bound(all(v), x)` → first `>= x`; `upper_bound` → first `> x`. Count of `x` in sorted v: `upper - lower`.
- `set.lower_bound(x)` is O(log n); `lower_bound(all(set), x)` is **O(n)**. Same for multiset/map.
- `multiset.erase(x)` removes **all** copies of x; use `ms.erase(ms.find(x))`.
- `priority_queue` is a max-heap; min-heap: `priority_queue<T, vector<T>, greater<T>>`.
- `accumulate(all(v), 0LL)` — the `0LL` matters.
- `(int)v.size() - 1` — `v.size() - 1` is unsigned and wraps when empty.
- `1 << k` is `int`; use `1LL << k` for k ≥ 31. `__builtin_popcountll` for `ll`.
- `memset` only for 0 / -1 / `0x3f` (int ≈ 1.06e9, sum of two still fits) .

## Stress testing (do this after the first unexplained WA)

```bash
# gen.py prints a random small test; brute.cpp is the obviously-correct slow solution
g++ -O2 -o sol sol.cpp && g++ -O2 -o brute brute.cpp
for i in $(seq 1 1000); do
  python3 gen.py $i > in.txt
  ./sol < in.txt > o1.txt; ./brute < in.txt > o2.txt
  if ! cmp -s o1.txt o2.txt; then echo "DIFF on seed $i"; cat in.txt; break; fi
done
```

```python
# gen.py
import random, sys
random.seed(int(sys.argv[1]))
n = random.randint(1, 6)
print(n)
print(*[random.randint(1, 10) for _ in range(n)])
```

Keep tests *tiny* (n ≤ 6, values ≤ 10): the smallest failing case is the easiest to debug by hand.
