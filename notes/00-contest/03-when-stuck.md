# When You're Stuck

## Constraints → target complexity

Budget ≈ 3·10^8 simple operations per second (less with maps/sets, recursion, cache misses).

| n up to | target | typical techniques |
|---|---|---|
| 8–11 | O(n!·n) | all permutations (`next_permutation`), brute force |
| 15–20 | O(2^n·n), O(3^n) at 15 | bitmask DP, subset enumeration, submask DP |
| 35–45 | O(2^(n/2)) | meet in the middle |
| 50–100 | O(n^4), O(n^3 log) | Floyd, flows, small Hungarian, DP with 3–4 dims |
| 300–500 | O(n^3) | interval DP, Floyd, Gaussian elimination, Hungarian |
| 2000–5000 | O(n^2) | 2D DP, all pairs, O(n^2) with bitset /32 for 10^4–10^5 |
| 10^5–2·10^5 | O(n log n), O(n log² n), O(n√n) | sort, BIT/segtree, binary search, Dijkstra, Mo's |
| 10^6 | O(n), O(n log n) small const | linear scans, sieve, prefix sums, two pointers, KMP/Z |
| 10^9–10^18 | O(log n), O(√n), O(cube-root) | math formula, binary search, matrix power, digit DP, factorization |

- **Sum of n over tests ≤ X** → per-test complexity can use X. Otherwise multiply by T!
- Two small parameters (e.g. n ≤ 10^5, k ≤ 20) → O(n·k) or O(n·2^k) DP is intended (ACPC 2022 *City Numbers*: k ≤ 20 colours on a tree).
- A weirdly specific bound is a hint: `n ≤ 15` (bitmask/interval brute), `values ≤ 200` (enumerate the value — ACPC 2018 *Dog Trouble*), `prime factors < 10^6` (trial division works), `p < 10^10, s ≤ 10^5` (s² can exceed p).
- Output modulo a prime → counting, combinatorics, DP; answer would be huge otherwise.
- Output a real number with 1e-6 → binary/ternary search on reals, geometry, probability.

## Keyword → technique

| if the problem says / looks like ... | try | where |
|---|---|---|
| minimize the maximum / maximize the minimum | binary search on answer + greedy/BFS check | §2.1 |
| "find the smallest X such that feasible" and feasibility is monotone | binary search on X | §2.1 |
| maximize ratio Σa/Σb | binary search λ: is max Σ(a − λb) ≥ 0 (Dinkelbach) | §2.1 |
| unimodal function of a real variable | ternary search / golden section | §2.1 |
| k-th smallest of something implicit | binary search on value, count ≤ mid | §2.1 |
| subarray sums/xors, count subarrays with property | prefix sums + hashmap / two pointers | §2.2, §2.3 |
| range add many times, query once | difference array | §2.2 |
| sum over all pairs/subarrays of f | contribution technique: count how often each element contributes | §2.2 |
| nearest greater/smaller, "visible", histogram | monotonic stack | §2.3 |
| sliding window min/max | monotonic deque | §2.3 |
| schedule jobs with deadlines/durations | sort by deadline + heap; exchange argument | §2.4 |
| choose k things to maximize something, n large | greedy + heap; check with exchange argument | §2.4 |
| n ≤ 40, subset sums | meet in the middle | §2.5 |
| connectivity under edge deletions, offline | reverse time → DSU additions | §3.1 |
| point update + range query | Fenwick / segment tree | §3.2–3.3 |
| range update + range query | lazy segment tree | §3.3 |
| static range min/gcd | sparse table | §3.4 |
| k-th element in dynamic set, order of key | PBDS ordered set / BIT walk | §3.4 |
| many range queries, offline, no updates, hard to merge | Mo's algorithm | §3.4 |
| count / optimize over sequences, choices at each step | DP; define state = what you need to know about the past | §4.1 |
| subsequence counting (e.g. palindromic subsequences) | interval DP with inclusion–exclusion | §4.1 |
| interleaving two strings, edit distance, LCS | 2D DP | §4.1 |
| merge adjacent / parenthesize / split interval | interval DP O(n^3); keep max AND min if signs | §4.1 |
| n ≤ 20 items, choose order/assignment | bitmask DP | §4.2 |
| count numbers in [L, R] with digit property | digit DP | §4.2 |
| tree, choose labels/colours per node with neighbour constraint | tree DP (keep best & second best) | §4.3 |
| answer for every root | rerooting DP | §4.3 |
| dp[i] = min_j (dp[j] + b_j·a_i) | convex hull trick / Li Chao | §4.4 |
| dp[k][i] = min_j dp[k−1][j] + cost(j,i), cost "Monge" | divide & conquer opt / Knuth | §4.4 |
| expected value, probability of winning, optimal stopping | linearity of expectation; DP over states backward | §4.5 |
| turns repeat in a cycle with probabilities | geometric series / solve linear equations | §4.5 |
| two players, optimal play, who wins | win/lose DP, Sprague–Grundy, find pattern by brute force | §4.5 |
| shortest path, unweighted / weights 0–1 / nonneg | BFS / 0-1 BFS / Dijkstra | §5.1 |
| shortest path with extra condition (fuel, horse, key, parity) | Dijkstra on (node, state) | §5.1 |
| distance to nearest of many sources | multi-source BFS/Dijkstra | §5.1 |
| path maximizing the minimum edge/vertex value | binary search + BFS, or max spanning tree, or modified Dijkstra | §5.1 |
| which sums are representable by coins (unbounded) | Dijkstra on residues mod smallest coin | §5.1 |
| minimum ratio spanning tree | binary search λ + MST | §5.2 |
| dependencies / ordering | topological sort, DAG DP | §5.2 |
| each node has one outgoing edge | functional graph: cycles + trees, binary lifting | §5.2 |
| boolean constraints "a or b" | 2-SAT | §5.3 |
| edges whose removal disconnects | bridges / articulation points | §5.3 |
| use every edge exactly once | Euler path (Hierholzer) | §5.3 |
| tree path queries, ancestors, distances | LCA / binary lifting / Euler tour + BIT / HLD | §5.4 |
| count paths in tree with property | centroid decomposition / small-to-large | §5.4 |
| assign workers to jobs one-to-one, min cost | Hungarian / min-cost flow | §6.3 |
| max pairs with compatibility | bipartite matching | §6.2 |
| min #paths covering all vertices of a DAG | n − max matching (split graph) | §6.2 |
| min #paths covering all **edges** of a DAG | min flow with lower bounds = Dilworth on edges | §6.1 |
| cut with costs, "choose set with dependencies, max profit" | min cut / project selection | §6.1 |
| grid with dominoes / chessboard parity | bipartite matching on black/white cells | §6.2 |
| gcd, lcm, divisibility, a·x + b·y = c | ext. gcd, CRT | §7.1–7.2 |
| sum of divisors, factorization of big n | sieve / trial division up to √n / Pollard rho | §7.2 |
| count arrangements, multisets, paths | stars & bars, Catalan, inclusion–exclusion, Burnside | §7.3 |
| linear recurrence with huge n | matrix exponentiation | §7.4 |
| system of linear (mod 2) equations, max xor subset | Gaussian elimination / xor basis | §7.4 |
| count pairs with sum = k for all k, pattern matching counts | FFT / NTT convolution | §7.5 |
| string equality of substrings fast | polynomial hashing | §8.1 |
| pattern occurrences / borders / periods | KMP prefix function, Z-function | §8.1 |
| rotations / cyclic shifts equal | minimal rotation (Booth) as canonical form | §8.2 |
| longest palindrome substring | Manacher | §8.2 |
| many patterns in a text | Aho–Corasick | §8.2 |
| distinct substrings, LCP of suffixes | suffix array + LCP | §8.2 |
| polygon area, orientation, lattice points | shoelace, cross product, Pick | §9.1–9.2 |
| smallest enclosing / outer boundary of points | convex hull | §9.2 |
| interactive: judge answers queries | binary search / information-theoretic planning / gcd tricks | §10.1 |
| evaluate expressions, mini-language | recursive descent parser | §10.2 |

## Stuck-on-an-idea checklist

1. **Re-read the statement.** Underline every constraint. Is something guaranteed (distinct values, connected, sorted, a < b)? That guarantee is usually *used*.
2. **Solve tiny cases by hand** (n = 1, 2, 3) and **write a brute force**; print answers for n = 1..10 and look for a pattern (then search OEIS-style in your head: powers of 2, Catalan, Fibonacci, n², C(2n,n)).
3. **What is the answer monotone in?** If "answer ≥ x is feasible" is monotone → binary search. Fixing the answer often turns optimization into a yes/no greedy.
4. **Fix one thing and iterate over it:** fix the maximum element, the left endpoint, the last chosen item, the root, the value of one variable (e.g. enumerate the max time t ≤ 200 then solve the rest).
5. **Sort** the input (by start, end, value, ratio). Many greedy/DP problems become easy after sorting by the right key; find the key with an exchange argument.
6. **Reverse** something: process time backward (deletions → additions), reverse the graph, think about the complement ("at least one" = total − "none").
7. **Change what you iterate over:** over values instead of indices, over divisors instead of multiples (harmonic sum n log n), over the answer instead of the input.
8. **Model as a graph:** states = nodes, moves = edges. Grid + extra state (direction, keys, used power-up) → BFS/Dijkstra on product graph.
9. **Model as a flow/matching** when there are "pairings", "assign each X to one Y", "each used at most once", "minimum number of chains/paths".
10. **Linearity of expectation:** E[sum] = Σ E[indicator]. Never compute the full distribution if only an expectation is asked.
11. **Bound the search:** the answer is small (≤ 60 when doubling, ≤ log n), the number of distinct values is small (≤ √(2n) distinct lengths if Σ = n), only O(√n) distinct `n / i` values.
12. **Look for invariants** (parity, sum mod k, xor) in games and constructive problems.
13. Think about the **special structure:** tree (unique paths, DP), DAG (topo order), bipartite, planar grid, permutation (cycles), convex polygon (O(n³) DP over vertices).
14. Still stuck after 20–30 min with no progress → switch problems, come back later.

## WA / RE / TLE checklist

- **Overflow**: `int * int`, sums of 10^5 values of 10^9, `1 << 40`, `INF + x`. When in doubt, `ll` everywhere.
- **Multi-test state** not reset (globals, adjacency lists, visited arrays, DSU).
- **Off-by-one**: inclusive vs exclusive ranges; 0- vs 1-indexed input; array size `n` vs `n + 1`.
- **Edge cases**: n = 1, empty set, all equal, all negative, answer 0, disconnected graph, self-loops, multi-edges, duplicate points, collinear points.
- **Reading**: multiple values per line, strings with spaces (`getline`), trailing spaces, Windows `\r`.
- **Output**: exact format/capitalisation ("Yes" vs "YES"), precision, one answer per line, `-0.0`.
- **Negative modulo**: `((a % m) + m) % m`. Subtraction in modular arithmetic: `(a - b + MOD) % MOD`.
- **Doubles compared with `==`**; use eps or exact integer arithmetic (cross products in `ll`).
- **Uninitialised** local variables / arrays (globals are zeroed, locals aren't).
- **Recursion depth** (10^6 deep DFS), **stack arrays** too big (`int a[10^7]` inside `main` → use global/vector).
- **TLE**: `endl` flushing, `map` where an array works, passing `vector` by value, O(n) `lower_bound` on `set`, `string +=` in a loop creating copies (`s = s + c` is O(n)), recomputing in loops, too many `long double`.
- **Dijkstra**: skip stale heap entries (`if (d > dist[u]) continue;`), else worst case is quadratic.
- **Comparator** not strict weak ordering → crash in `sort`.
- **Wrong algorithm**: greedy without proof → stress test against brute force.
