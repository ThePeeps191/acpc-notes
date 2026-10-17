# Complete Search, Meet in the Middle, Square Root, Randomization

## Brute force done right

- Enumerate subsets: `for (int m = 0; m < (1 << n); ++m)`. Permutations: sort, then `do { } while (next_permutation(all(p)));` (n ≤ 10: 3.6M perms).
- **Backtracking with pruning:** order choices (most constrained first), prune with an optimistic bound (current + best possible remaining ≤ best found ⇒ stop), memoize repeated states.
- **Iterate over the small dimension:** if one of the parameters is ≤ 20–200 (number of colours, max value, number of properties), loop over it explicitly.
- **Search the answer space instead of the input space** when the answer is small (e.g. answer ≤ 60 because values double).
- Before brute forcing, count states × transitions and compare with 3·10^8.

## Meet in the middle

Split n ≤ 40 items into halves, enumerate 2^(n/2) each, combine with sort + two pointers/binary search/hash map.

```cpp
// number of subsets with sum <= S (n <= 40)
ll countSubsets(const vector<ll>& a, ll S) {
    int n = sz(a), h = n / 2;
    auto gen = [&](int l, int r) {
        vector<ll> s = {0};
        for (int i = l; i < r; ++i) {
            int k = sz(s);
            rep(j, 0, k) s.push_back(s[j] + a[i]);
        }
        sort(all(s)); return s;
    };
    vector<ll> L = gen(0, h), R = gen(h, n);
    ll cnt = 0; int j = sz(R) - 1;
    for (ll x : L) {                       // L ascending -> pointer in R only moves down
        while (j >= 0 && x + R[j] > S) --j;
        cnt += j + 1;
    }
    return cnt;
}
```

Also: 4-sum = pair sums of two halves; bidirectional BFS on huge implicit graphs (puzzle states) — expand from both ends, stop when frontiers meet.

## Square root decomposition

- **Blocks of size B ≈ √n:** each block stores an aggregate (+ lazy tag). Update/query touch O(n/B) full blocks + O(B) loose elements. Handy when the operation is weird (e.g. "add x to range, count elements < y in range": keep each block sorted → O(√n log n)).
- **Heavy/light by frequency:** values occurring > √n times are "heavy" (at most √n of them) → precompute for heavy, brute force for light.
- **Σ of sizes = n ⇒ at most O(√n) distinct sizes.** (Knapsack with Σ w = n has O(√n) distinct weights → bounded knapsack per distinct weight.)
- **Divisor/quotient blocks:** `n / i` takes O(√n) distinct values; iterate blocks:

```cpp
// sum_{i=1}^{n} f(n / i) grouped: for each block [l, r], n / i == q
ll sumQuotients(ll n) {
    ll res = 0;
    for (ll l = 1, r; l <= n; l = r + 1) {
        ll q = n / l; r = n / q;
        res += (r - l + 1) * q;     // replace q by f(q), (r-l+1) by the weight of [l, r]
    }
    return res;
}
```

- **Mo's algorithm** for offline range queries: §3.4.
- **Rebuild periodically:** buffer updates; every √q updates rebuild the static structure; queries check the static structure + the buffer.

## Randomization

- **Random shuffle** before inserting into structures that degrade on sorted input (treap keys, BST, hull insertion).
- **Random sampling** to find a majority/frequent element: an element occupying ≥ 1/k of the array is hit with probability ≥ 1/k per sample; 30·k samples → failure ≈ e^-30.
- **Hashing with random base/mod** (§8.1); **xor-hashing of sets/multisets**: assign each value a random 64-bit number; set equality ⇔ xor (or sum) of hashes equal. Subarray is a permutation of 1..k ⇔ sum of hashes of subarray = prefix-hash of {1..k}.
- **Random restarts / simulated annealing** for optimization problems with no clear structure (last resort; tune by time limit with `clock()`).
- `mt19937_64 rng(seed)`; never `rand()` (max 32767 on some platforms).

## Iterative deepening / search with memo

- Game/puzzle with small depth: IDDFS or BFS over states stored in hash map (encode state into a 64-bit integer).
- **Memoized recursion over states** = DP. If the state space is unclear, write the recursive brute force, then add a `map<State, Answer>` and see if it's fast enough.
