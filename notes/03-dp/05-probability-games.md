# Probability, Expected Value, Games

ACPC has a probability/EV problem at difficulty 5–8 most years (Balls and Bins, Gunslingers, Concentration). They are almost always **DP over a small state space**, solved backward from terminal states.

## Core facts

- **Linearity:** E[X + Y] = E[X] + E[Y] always (even dependent). Write the quantity as a sum of indicators: E[#fixed points of random permutation] = n·(1/n) = 1; E[#inversions] = n(n−1)/4.
- E[X] for X ≥ 0 integer: Σ_{k≥1} P(X ≥ k). E[max] via P(max ≤ k) = Π P(X_i ≤ k).
- **Geometric:** success prob p per trial ⇒ expected trials 1/p. **Coupon collector:** n·H_n = n(1 + 1/2 + … + 1/n).
- Independent events: P(A ∩ B) = P(A)P(B). Conditional: P(A | B) = P(A ∩ B)/P(B). Bayes: P(A | B) = P(B | A)P(A)/P(B).
- **Gambler's ruin** (fair): start at a, absorb at 0 or N ⇒ P(reach N) = a/N, expected steps a(N − a). Biased with ratio r = q/p: P(reach N) = (1 − r^a)/(1 − r^N).
- Sum of probabilities over all outcomes must be 1 — assert it in your DP to catch bugs.
- Random permutation: element i ends before j with prob 1/2; i is the max of a random set of size k with prob 1/k.

## Expected value DP

`E[s] = cost(s) + Σ_t P(s → t)·E[t]`, terminal states have E = 0. Compute in an order where t is done before s (e.g. "number of remaining items" decreases).

- **Self-loops** (state can stay the same with prob p): E = (c + Σ_{t ≠ s} P(t)E[t]) / (1 − p).
- **Optimal stopping / decisions** (ACPC 2021 *Balls and Bins*): `V[s] = max(value if stop now, Σ P(t)·(reward + V[t]))`. The state must capture everything that affects future probabilities — e.g. (#bins with 1 ball, #bins with 2 balls); the number of balls thrown is then determined.
- **Forward probability DP** when you need the distribution: `P[t] += P[s]·p(s → t)`; answer = Σ over final states.
- **Probability of winning with optimal play:** `W[s] = max over moves of Σ P(outcome)·W[next]` (or 1 − opponent's W when turns alternate).
- **Cycles in the state graph**:
  - A *cyclic chain* (turns rotate among players until something happens): unroll. If `x_1 = a_1 + b_1 x_2, …, x_k = a_k + b_k x_1` then `x_1 = (a_1 + b_1 a_2 + b_1 b_2 a_3 + … ) / (1 − b_1 b_2 ⋯ b_k)`, then back-substitute. (Gunslingers: within a fixed set of survivors, misses rotate the turn; hits go to a smaller set, already solved.)
  - General cycles: Gaussian elimination on the linear system (§7.4), O(n³) for n ≤ ~500.
  - Infinite processes with decreasing probability: iterate until change < 1e-12 (value iteration) when the system is contractive.

```cpp
// x[i] = a[i] + b[i] * x[(i+1) % k]  (cyclic chain), returns all x. Requires prod(b) != 1.
vector<double> solveCycle(const vector<double>& a, const vector<double>& b) {
    int k = sz(a); double num = 0, mul = 1;
    rep(i, 0, k) { num += mul * a[i]; mul *= b[i]; }
    vector<double> x(k);
    x[0] = num / (1 - mul);
    for (int i = k - 1; i >= 1; --i) x[i] = a[i] + b[i] * x[(i + 1) % k];
    return x;
}
```

### Expected value modulo p

"Output E as P·Q⁻¹ mod 998244353": do the same DP with modular arithmetic; every division by an integer d becomes multiplication by `inv(d)` (§7.1). Probabilities like 1/n become `inv(n)`. `1/(1 − p)` becomes `inv((1 − p + MOD) % MOD)`.

### Precision with doubles

Use `double` (or `long double`) and print 9+ decimals. Avoid computing tiny probabilities as products of thousands of terms (underflow → use logs). Subtracting nearly equal numbers loses precision — reformulate (`1 − (1 − p)^n` via `-expm1(n * log1p(-p))`).

## Combinatorial games

**Win/lose DP on a DAG of positions:** a position is winning iff some move leads to a losing position; terminal positions are losing (normal play: player unable to move loses). Compute by memoized recursion.

**Games with cycles (draws possible) — retrograde BFS:** start from terminal positions; a position becomes LOSE when all its successors are WIN (count remaining successors), WIN when any successor is LOSE; positions never labelled are DRAW.

```cpp
// g = moves (position -> next positions). res: 1 = win, -1 = lose, 0 = draw (for player to move)
vector<int> retrograde(const vector<vector<int>>& g) {
    int n = sz(g); vector<vector<int>> rg(n); vector<int> deg(n), res(n, 0);
    rep(v, 0, n) { deg[v] = sz(g[v]); for (int u : g[v]) rg[u].push_back(v); }
    queue<int> q;
    rep(v, 0, n) if (deg[v] == 0) { res[v] = -1; q.push(v); }
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : rg[u]) if (res[v] == 0) {
            if (res[u] == -1) { res[v] = 1; q.push(v); }
            else if (--deg[v] == 0) { res[v] = -1; q.push(v); }
        }
    }
    return res;
}
```

**Sprague–Grundy:** for a sum of independent impartial games, `grundy(position) = mex{grundy(next)}`; the whole game is losing iff the xor of the components' Grundy values is 0.

- **Nim:** Grundy of a heap of size x is x → first player wins iff xor ≠ 0.
- **Subtraction game** (remove 1..k): g(x) = x mod (k+1).
- **Splitting games** (a heap splits into two): g = mex over splits of g(a) xor g(b).
- **Staircase Nim:** only odd-indexed steps matter (xor of piles on odd steps).
- **Misère Nim:** same as Nim unless all heaps ≤ 1, then first player wins iff the number of heaps of size 1 is even.
- **Grundy values often become periodic**: compute the first few hundred by brute force and look for the period.

**Two-player optimisation games** (both maximise own score, take from ends of an array): `dp[l][r]` = best (my score − opponent's score) for the player to move on [l, r] = max(a_l − dp[l+1][r], a_r − dp[l][r−1]).

**Strategy-stealing / symmetry**: first player often wins by mirroring (central symmetry on a board, pairing moves). When stuck, brute force small n and print the win/lose table — a pattern (parity, mod k, powers of 2) usually appears.
