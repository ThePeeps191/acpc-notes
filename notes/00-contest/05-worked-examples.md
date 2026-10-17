# Worked Patterns from Past ACPC Problems

Short solution sketches of past ACPC problems, grouped by the *recognition trigger*. The point is to train "this smells like X". Read the trigger, guess the technique, then read the sketch.

## Binary search on the answer

**Hockey Fans (2024, 4.4).** *Largest D such that M disjoint windows of length S all have values ≥ D.* Trigger: "largest value such that it is possible". Feasibility is monotone in D. Check(D): scan left to right with a counter of consecutive values ≥ D; whenever it reaches S, take a window and reset the counter. Count ≥ M ⇒ feasible. Binary search D over the sorted distinct values. O(N log N).

**Hired Help (2020, 5.5).** *Max employees, each must repair exactly K shoes (1 minute each) before the shoes' deadlines.* Trigger: "maximum number such that …". Check(k): take the k·K shoes with the latest deadlines (exchange argument: a later deadline is never worse); sort them ascending; shoe number i (0-indexed) is repaired in minute ⌊i/k⌋ + 1 at the earliest, so feasible iff `d_i ≥ ⌊i/k⌋ + 1` for all i. Binary search k.

**Arachnophobia (2018, 4.0).** *Maximise the minimum spider distance along a path that must take ≤ T seconds.* Trigger: maximin + another constraint. Multi-source Dijkstra from all spiders gives `sd[v]`. Binary search D: Dijkstra from s using only vertices with `sd ≥ D`; feasible iff dist(t) ≤ T. O((n + m) log n · log V).

**Horse Snatchers (2023, 6.8).** *Slowest horse that still reaches town n by time T; entering a snatcher town loses the horse, then you walk.* Faster horse never hurts ⇒ binary search the speed. Check: Dijkstra over states (town, still riding) where riding edges cost d/speed and walking edges cost d/w; entering a snatcher town switches the state to walking.

**Buildin' Roads (2023, 3.3).** *Spanning tree minimising Σpitch / Σgravel.* Ratio objective ⇒ binary search λ, MST with weights `pitch − λ·gravel`, feasible iff total ≤ 0.

## Shortest paths in disguise

**Braaaains (2022, 5.8).** *Container sizes (unbounded); which order sizes are exactly packable?* Classic coin reachability for huge targets: Dijkstra on residues modulo the smallest size a (§5.1); X is packable iff `d[X mod a] ≤ X`. O(a·k log a).

**Dacey the Dice (2016) / Slippery Floor (2024).** BFS where a node is (cell, extra state): the die orientation (24 states) or "where you stop after sliding".

## DP

**Balls and Bins (2021, 5.1).** *Optimal stopping; throwing into a bin with 3 balls loses everything.* Because every successful throw earns p_i and failure loses everything, the money earned so far is just the prefix sum of p up to t = #balls thrown. State = (#bins with 1, #bins with 2, #bins with 3) determines t. `V(s) = max(P_t, Σ_{outcomes} prob · V(next))` with the "full bin" outcome contributing 0. O(N³) states, O(1) transitions, compute in decreasing t.

**City Numbers (2022, 4.4).** *Colour a tree with 1..k, adjacent different, minimise Σ colours.* Tree DP `dp[v][c]`, merging a child needs `min_{c' ≠ c} dp[child][c']` = best or second best (§4.3). O(nk).

**Palindromic Naming (2016, 4.1).** Count palindromic subsequences: interval DP with inclusion–exclusion (§4.1).

**Homework (2018, 6.5).** *Is s an interleaving of s1 and s2?* 2D boolean DP over (i, j) — 25·10^6 states, tight but fine with a `vector<char>` and a tight loop.

**Trimming Convex Polygons (2018, 6.9).** *Keep a subset of convex polygon vertices: gain 2·area of kept polygon + values of discarded vertices.* Kept vertices form a subsequence in cyclic order; area = fan of triangles from the first kept vertex. dp over (first kept, last kept) adding one triangle per step. O(n³) for n = 200.

**Maximal Parentheticals (2022, 8.0).** Parenthesise `d op d op …` (N ≤ 15) to maximise the value. Twist: writing `1 (- 2)` turns the binary minus into a unary minus and creates an *implicit multiplication*, so sub-expressions can be multiplied. Interval DP over token ranges storing the max and min achievable value (multiplication and subtraction of negatives flip extremes), with separate states for "interval starts with a unary sign". Read such statements for every way the given rules can be combined — the hard part is the case analysis, not the DP.

## Flows and matching

**Dog Trouble (2018, 5.8).** *Assign N dogs to distinct bowls minimising Σ(t − t_i) where t is the max.* Mixed max/sum objective ⇒ enumerate t (≤ 200 values), Hungarian maximising Σt_ij over allowed pairs (§6.3). 200 × 50³.

**Escape Routes (2022, 8.3).** *Minimum number of 0 → V−1 paths in a DAG covering every edge.* Flow with lower bound 1 on every edge and infinite capacity, minimise the flow value ⇒ min flow with lower bounds (§6.1). (Equivalently: Dilworth on edges.)

## Number theory / interactive

**Four Questions (2024, 7.1).** *Find a prime p < 10^10 from 4 replies of s² mod p, s ≤ 10^5.* With s near 10^5, s² ≥ p (usually), so the reply r is informative: p | s² − r. g = gcd of the values s_i² − r_i is a multiple of p; candidates are prime factors q of g with q > max(r_i) and `s_i² mod q == r_i` for all i (factor g by trial division up to 10^5). Subtlety: for **small p** several primes can explain the same replies (s² mod p is tiny) — use the remaining queries adaptively, picking an s whose predicted replies differ between the surviving candidates. Lesson: count candidates after each query and spend queries to split them.

**Tricky Factoring (2022, 6.7).** *Count b with ax² + bx + c factorable over integers (a, c ≤ 10^16, prime factors < 10^6).* (dx + e)(fx + g): a = df, c = eg, b = dg + ef. Enumerating divisor pairs is too slow (τ up to ~4·10^4 each). Key observation: u = dg and v = ef satisfy u·v = ac, and *every* divisor u of ac splits as d·g with d | a, g | c. So |b| ranges over {u + ac/u : u | ac}, which is injective for u ≤ √(ac). Answer = 2·⌈τ(ac)/2⌉ (sign ±), τ from the merged factorisations of a and c (matches all samples: 2, 18, 128, 486). Lesson: **look for an invariant product/sum that collapses the double enumeration.**

**Outlaw Numbers (2023, 4.0).** Iterate n → σ(n) − n at most 32 times; each step needs factorisation by trial division / Pollard.

## Strings

**Password Rotation (2020, 6.3).** *Do any two passwords match up to rotation or reversed rotation?* Canonical form = min(minRotation(s), minRotation(reverse s)) (§8.2); insert into a hash set; any repeat ⇒ "Yes". Total length 10^6 ⇒ O(total).

**Matchings (2017, 7.5).** *For every placement of a small image inside a big one, count equal pixels; report the best placements.* For each colour c: correlation of the big image's indicator with the small image's indicator — FFT after flattening with the big image's width (§7.5).

## Offline / data structures

**Supply Routes (2022, 3.5).** Edges get destroyed; answer connectivity queries. Reverse time: start from the final graph, add edges back with DSU, answer queries in reverse.

**Topographic Isolation (2024, 4.0).** Nearest index with height ≥ h on each side: monotonic stack from both directions; peak's isolation = min distance (−1 if none).

**Maximal Sequences (2017, 6.2).** *Longest run starting at i using only values in B (Σ|B| ≤ 10^5).* Offline by i descending; keep for each value its next occurrence ≥ i in an ordered set; the answer is the first next-occurrence of a value not in B — check the earliest |B| + 1 entries of the set.
