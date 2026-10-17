# ACPC 2016–2024: Every Problem → Technique

Kattis difficulty in brackets. Technique = the intended core idea (my analysis of the statements; `?` = likely but unverified). Use this to calibrate: what does a 4.0 vs 6.5 ACPC problem look like?

| year | problem [diff] | core technique |
|---|---|---|
| 2016 | Mirror Images [1.6], Soylent [1.7], Bits [2.2] | implementation |
| | Wood Cutting [2.5] | sort ascending (shortest processing time first), prefix sums |
| | Dacey the Dice [3.3] | BFS on (cell, die orientation) — 24 orientations |
| | Flower Garden [3.6] | prefix of Manhattan distances + sieve for primes |
| | Palindromic Naming [4.1] | count palindromic subsequences, interval DP O(n²) mod p (§4.1) |
| | City Destruction [5.6] | linear DP over buildings with state "was neighbour destroyed before me" |
| | Modelling Problems [6.8] | parse a mini-language, brute force over unknown 0/1 values |
| 2017 | Musical Scales [1.6], Tetration [1.7] | implementation / math (x = a^(1/a)) |
| | Quantum Superposition [2.2] | DAG DP: set of reachable path lengths (bitset), combine two sets |
| | Anthony and Diablo [2.3] | geometry: max area for perimeter is a circle |
| | Divide by 100 [3.2] | big-number string manipulation |
| | Concentration [4.8] | probability / game simulation reasoning ? |
| | Lane Switching [5.5] | maximize the minimum (bottleneck path) over intervals: Dijkstra/BFS ? |
| | Maximal Sequences [6.2] | offline by start index; ordered set of next occurrences per value; check first m+1 |
| | Matchings [7.5] | 2-D pattern matching counts via FFT convolution, one FFT per colour (§7.5) |
| 2018 | Early Winter [2.0], Eating Out [2.5] | implementation / pigeonhole counting |
| | Arachnophobia [4.0] | multi-source Dijkstra from spiders + binary search D + Dijkstra s→t |
| | Dog Trouble [5.8] | enumerate max time t (≤ 200), assignment (Hungarian) maximizing Σt_i (§6.3) |
| | PUBNite [5.8] | geometry + binary/ternary search on time |
| | Exploding Kittens [6.4] | simulation, skipping irrelevant cards ? |
| | Homework [6.5] | string interleaving DP O(len1·len2) (bitset if tight) |
| | Trimming Convex Polygons [6.9] | DP over convex polygon vertices O(n³) (fix first kept vertex, triangle fan) |
| 2020 | Test Drive [1.9], Laptop Stickers [1.9], Warring Scoring [2.8] | implementation |
| | Path Crossings [2.9] | hashing (x,y,t) / sort events |
| | Straights [2.9] | greedy on value counts |
| | Listen To Your Boss [3.9] | tree ancestor queries: Euler tour tin/tout (§5.4) |
| | Wormhole Extreme!!! [4.4] | kinematics; minimum initial velocity via v² recursion / binary search |
| | Hired Help [5.5] | binary search #employees + greedy deadline check |
| | Monochromatic Minesweeper [6.2] | constructive / search with pruning ? |
| | Password Rotation [6.3] | canonical form = min rotation of s and reverse(s) (Booth), hash set (§8.2) |
| | Wrapping Trees [7.3] | fractal → convex hull of the self-similar set; geometry |
| | Best Investing [7.8] | math / greedy over deposit–withdraw schedule ? |
| 2021 | Finding An A [1.2], GCVWR [1.4], Square Peg [1.5], Simone [1.8], Colour Wars [2.3] | implementation |
| | Snakes and Masters [2.9] | Fibonacci DP mod 10^6 |
| | Bitmask [3.1] | string/bit matching with DP ? |
| | Menu Updates [3.3] | maintain smallest available number: set/heap |
| | Associative Exponents [3.4] | compare a^(b^c) vs (a^b)^c ⇔ b^c vs b·c, careful with overflow |
| | Ghostbusters 3 [3.8] | non-crossing assignment: sort + greedy/DP ? |
| | Wonky Pizza [4.2] | polar area ∫ r²/2 dθ closed form + binary search the angle |
| | Beep Code [4.6] | bit decomposition of a sum of square waves; reason level by level |
| | Balls and Bins [5.1] | optimal-stopping expected-value DP over (#bins with 1, #bins with 2) (§4.5) |
| | Swapping Places [8.1] | constructive: permutation routing on spanning tree, process leaves first |
| | Compression [8.2] | DP over block boundaries with careful cost precomputation |
| 2022 | Undead or Alive [1.3], Ordinals [1.8], Count Doubles [1.8] | implementation |
| | Contingency Planning [2.1] | Σ_k n!/(n−k)! with cap |
| | Simple Factoring [2.4] | discriminant b² − 4ac is a perfect square |
| | Maximum Fix [3.4] | each element votes for the one shift that fixes it; count array |
| | Supply Routes [3.5] | offline: reverse edge deletions → DSU unions (§3.1) |
| | Fight to Survive [3.8] | greedy / simulation ? |
| | City Numbers [4.4] | tree DP dp[v][colour], keep best & second best child value (§4.3) |
| | Long Walk [5.5] | implicit graph i ↔ 2i mod 2^n: bit manipulation, LCA-like on binary strings |
| | Braaaains [5.8] | Frobenius/coin reachability: Dijkstra on residues mod smallest size (§5.1) |
| | Tricky Factoring [6.7] | b = u + ac/u over divisors u of ac ⇒ answer 2·⌈τ(ac)/2⌉; factor via sieve |
| | Maximal Parentheticals [8.0] | interval DP tracking max and min; parentheses can create unary minus + implicit multiplication |
| | Escape Routes [8.3] | min #paths covering all DAG edges = min flow with lower bounds (§6.1) |
| 2023 | Buildin' Fences [1.5] | implementation |
| | Flying Straight [3.0] | trigonometry (arc vs tangent) |
| | Buildin' Roads [3.3] | minimum ratio spanning tree: binary search λ + MST (§2.1, §5.2) |
| | OBD Codes [3.9] | pattern matching on event streams ? |
| | Counting Cacti [4.0] | grid parsing + flood fill components |
| | Outlaw Numbers [4.0] | aliquot sequence: sum of proper divisors via factorization, ≤ 32 steps |
| | Wrangler Rating [4.5] | careful simulation of rules |
| | Circuit Game [5.2] | graph connectivity under switch removal + budget ? |
| | Dice Tables [5.2] | counting multisets (stars and bars) over overlapping face ranges, mod 10^9+9 |
| | Desert Strike [5.4] | lattice counting of a rasterized rotated square (math) |
| | High Noon Language Showdown [5.5] | brute force over n with exact integer ceil(log2) |
| | Temporal Turmoil [6.7] | clock-hand alignment math with exact fractions |
| | Horse Snatchers [6.8] | binary search horse speed + Dijkstra on (node, has horse) |
| | Gunslingers [7.8] | probability DP over survivor sets with cyclic turns (geometric series) |
| 2024 | Snowfall [1.6], Melting Snow [1.7], Handheld Fan [2.2] | implementation |
| | Wrapping Snowballs [3.5] | ? |
| | Alpine Agility [3.8] | grid DP (moves S/E/SE) |
| | Slippery Floor [3.8] | BFS where a move slides until hitting an obstacle |
| | Topographic Isolation [4.0] | monotonic stack: nearest ≥ on each side (§2.3) |
| | A Little to the Right [4.2] | count distinct sorted orders among properties with all-distinct values |
| | Hockey Fans [4.4] | binary search on D + greedy left-to-right placement |
| | Uniform Variation [4.7] | brute force: groups of all-different are ≤ 4; enumerate small subsets |
| | Snow Way Out [6.7] | interactive, 6 comparisons on 10×10 grid: offline minimax search for a strategy |
| | Four Questions [7.1] | interactive: s² mod p ⇒ p divides s² − r; gcd of such values (§10.1) |
| | Speedy Slopes [7.2] | shortest path with speed state on grid ? |

**Patterns to take away**

- Binary search on the answer appears almost every year (Arachnophobia, Hired Help, Hockey Fans, Horse Snatchers, Buildin' Roads).
- Dijkstra variants: multi-source, state-augmented, residue graph (Braaaains).
- Probability/expected value DP appears most years at 5–8 difficulty.
- One flow/matching problem every year or two (Dog Trouble, Escape Routes).
- Interactive problems appeared in 2024 (two of them) — read §10.1 before the contest.
- Number theory with "all prime factors < 10^6" or big-number twists.
