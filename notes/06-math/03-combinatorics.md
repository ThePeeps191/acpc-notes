# Counting

## The standard toolbox

| objects | count |
|---|---|
| sequences of length k from n symbols | n^k |
| arrangements of k distinct out of n | n!/(n−k)! |
| subsets of size k | C(n, k) |
| **multisets** of size k from n types (stars & bars) | C(n + k − 1, k) |
| solutions of x_1 + … + x_n = k, x_i ≥ 0 | C(n + k − 1, n − 1) |
| same with x_i ≥ 1 | C(k − 1, n − 1) |
| same with x_i ≤ c | inclusion–exclusion: Σ_j (−1)^j C(n, j) C(k − j(c+1) + n − 1, n − 1) |
| permutations of a multiset (counts c_i) | n! / Π c_i! |
| lattice paths (0,0) → (a,b) with steps R/U | C(a + b, a) |
| paths that never go below the diagonal | Catalan-type (reflection, below) |
| surjections n → k | Σ_j (−1)^j C(k, j)(k − j)^n = k!·S(n, k) |
| derangements D_n | (n − 1)(D_{n−1} + D_{n−2}), D_n ≈ n!/e |
| labelled trees on n vertices | n^(n−2) (Cayley); forests with k given roots: k·n^(n−k−1) |
| binary strings of length n with no two adjacent 1s | Fibonacci F_{n+2} |

**Catalan** C_n = C(2n, n)/(n+1) = C(2n, n) − C(2n, n+1): 1, 1, 2, 5, 14, 42, 132, 429, 1430, 4862, 16796. Counts balanced bracket sequences, full binary trees with n+1 leaves, triangulations of an (n+2)-gon, monotone lattice paths not crossing the diagonal, non-crossing matchings of 2n points, stack-sortable permutations. Recurrence C_{n+1} = Σ C_i C_{n−i}.

**Reflection principle:** a path that touches the forbidden line can be reflected (from its first touch) to a path starting at the mirrored start point; so #bad = #paths from the mirrored start. **Ballot:** sequences of n R-steps and m U-steps (m ≤ n) in which every prefix has #U ≤ #R: C(n + m, m) − C(n + m, m − 1) (n = m gives Catalan). Strictly more R than U in every non-empty prefix: (n − m)/(n + m)·C(n + m, m).

**Stirling numbers of the 2nd kind** S(n, k) (partitions of n labelled items into k non-empty unlabelled groups): S(n, k) = k·S(n−1, k) + S(n−1, k−1). **Bell numbers** B_n = Σ_k S(n, k): 1, 1, 2, 5, 15, 52, 203, 877, 4140. **Stirling 1st kind** (permutations with k cycles): c(n, k) = (n−1)c(n−1, k) + c(n−1, k−1).

**Integer partitions** p(n) (unordered sums): p(n) = 1, 1, 2, 3, 5, 7, 11, 15, 22, 30, 42 …, p(100) ≈ 1.9·10^8. DP: `for part in 1..n: for s = part..n: p[s] += p[s − part]` (coin-change counting).

## Identities

- Pascal C(n, k) = C(n−1, k−1) + C(n−1, k); symmetry C(n, k) = C(n, n−k); Σ_k C(n, k) = 2^n; Σ_k (−1)^k C(n, k) = 0.
- **Hockey stick:** Σ_{i=k}^{n} C(i, k) = C(n+1, k+1). **Vandermonde:** Σ_j C(a, j) C(b, k−j) = C(a + b, k).
- k·C(n, k) = n·C(n−1, k−1). Σ_k k·C(n, k) = n·2^(n−1).
- C(n, k) is maximal at k = n/2; C(60, 30) ≈ 1.2·10^17 (fits in `ll`), C(67, 33) overflows `ll`.
- Number of monotone paths in a grid avoiding forbidden cells: sort cells, dp[i] = paths to cell i avoiding earlier forbidden ones = C(to i) − Σ_{j before i} dp[j]·C(j → i).

## Inclusion–exclusion

|A_1 ∪ … ∪ A_n| = Σ over nonempty S of (−1)^(|S|+1)·|intersection of A_i for i ∈ S|. Typical shape: count objects violating **none** of n conditions = Σ over all S of (−1)^|S|·(#objects violating at least all conditions in S). With symmetric conditions this collapses to Σ_j (−1)^j C(n, j)·f(j).

- Count integers ≤ N divisible by none of primes p_1..p_k: Σ over subsets (−1)^{|S|} ⌊N / Π p⌋ (k ≤ 15 → 2^k terms; prune when the product exceeds N).
- Count arrangements where at least one of p properties holds (ACPC 2024 *A Little to the Right*): either inclusion–exclusion over subsets of properties, or observe structure (a property with all-distinct values defines exactly one order; answer = number of distinct such orders).
- Inclusion–exclusion over subsets of n ≤ 20 with SOS (§4.2) when the "intersection" depends on a mask.

## Burnside / Pólya (counting up to symmetry)

Number of distinct objects = (1/|G|) Σ_{g ∈ G} fix(g). For colourings, fix(g) = k^{c(g)} with c(g) = number of cycles of g on positions.

- **Necklaces** (rotations only), n beads, k colours: (1/n) Σ_{d|n} φ(d) k^{n/d}.
- **Bracelets** (rotations + reflections): necklaces/2 + (n odd: k^{(n+1)/2}/2; n even: (k^{n/2} + k^{n/2+1})/4).
- Cube faces with k colours (rotations): (k^6 + 3k^4 + 12k^3 + 8k^2)/24.
- Modulo p: divide by |G| using an inverse (p must not divide |G|).

## Probability-flavoured counting

- Expected number of distinct values when drawing k times uniformly from n: n(1 − (1 − 1/n)^k).
- Random permutation: expected cycles H_n; probability that 1 and 2 are in the same cycle 1/2.
- Birthday: collision among k random values in [0, N) likely once k ≈ √N (so 64-bit hashes of 10^6 strings are safe, 32-bit are not).
