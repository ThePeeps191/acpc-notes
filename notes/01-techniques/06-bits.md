# Bit Manipulation and Bitset

## Tricks

| expression | meaning |
|---|---|
| `x & -x` | lowest set bit |
| `x & (x - 1)` | clear lowest set bit; `== 0` ⇔ x is a power of two (x > 0) |
| `__builtin_popcountll(x)` | number of set bits |
| `__builtin_ctzll(x)` / `__builtin_clzll(x)` | trailing / leading zeros (undefined for 0!) |
| `63 - __builtin_clzll(x)` | floor(log2 x) (`__lg(x)` also works) |
| `x >> k & 1` | k-th bit |
| `x ^ (1LL << k)` | toggle k-th bit |
| `(1LL << n) - 1` | mask of n ones |
| `a + b == (a ^ b) + 2 * (a & b)` | sum decomposes into xor + carries |
| `a ^ b ^ b == a` | xor cancels; prefix xor gives range xor |

```cpp
// iterate all submasks of mask (descending), including 0: total over all masks is 3^n
void subs(int mask) {
    for (int s = mask; ; s = (s - 1) & mask) {
        // use s
        if (s == 0) break;
    }
}
// iterate all supersets of mask within n bits
void supers(int mask, int n) {
    for (int s = mask; s < (1 << n); s = (s + 1) | mask) { /* use s */ }
}
// next mask with the same popcount (Gosper's hack)
ll nextSamePop(ll x) { ll c = x & -x, r = x + c; return (((r ^ x) >> 2) / c) | r; }
```

## XOR facts

- XOR of 0..n: `n % 4 == 0 ? n : n % 4 == 1 ? 1 : n % 4 == 2 ? n + 1 : 0`.
- Maximum xor of two numbers in a set: binary trie, greedily go to the opposite bit (O(30) per query).
- Maximum xor of any subset / k-th xor / "is x representable": **linear basis** (§7.4).
- Per-bit independence: many problems on sums of xors/ands/ors split into 30 independent problems, one per bit (count pairs with bit k set in a_i ^ a_j = cnt0·cnt1).
- `a | b` and `a & b`: `a + b = (a | b) + (a & b)`.

## Bitset

`bitset<N>` operations are N/64 word ops — 64× speedup for boolean DP.

```cpp
// subset-sum reachability up to MAXS: O(n * MAXS / 64)
const int MAXS = 100001;
bitset<MAXS> reachable(const vector<int>& w) {
    bitset<MAXS> b; b[0] = 1;
    for (int x : w) b |= b << x;
    return b;
}
```

- Transitive closure / reachability in a DAG: `reach[v] |= reach[u]` in reverse topo order: O(n·m/64).
- Counting triangles: for each edge (u,v): `(adj[u] & adj[v]).count()`.
- Sets of reachable path lengths in a DAG (ACPC 2017 *Quantum Superposition*): `len[v] |= len[u] << 1`.
- O(n²) DP where the transition is an OR/AND over a row (LCS-like boolean, interleaving check like ACPC 2018 *Homework*) can often be done row-by-row with shifts.
- `_Find_first()`, `_Find_next(i)` iterate set bits fast. Size must be a compile-time constant; for dynamic sizes pick the max or template over sizes.

## Gray code

`g = i ^ (i >> 1)`: consecutive values differ in one bit — enumerate subsets toggling one element at a time (useful when add/remove of an item is O(1) but recomputation is expensive).
