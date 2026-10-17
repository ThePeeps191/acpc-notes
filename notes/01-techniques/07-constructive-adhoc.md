# Constructive and Ad-hoc Problems

Div2 C – Div1 C problems are very often "observation + simple implementation". Process:

1. **Brute force small cases** (n ≤ 8) and *print* the answers / an optimal construction. Most patterns are visible immediately (answer depends on parity, n mod 3, whether n is a power of 2, …).
2. **Find a lower bound** (each operation fixes at most one inversion / changes the sum by at most 2 / reduces the number of components by one) and then **construct a solution meeting it**.
3. **Necessary conditions first** (sum parity, total divisible by k, counts of each value), then check with the brute force whether they are also sufficient.

## Invariants and monovariants

- Quantities that never change: sum mod k, parity of the number of inversions (each swap flips it), xor of all elements, colour counts on a chessboard (dominoes cover one black + one white), Σ x_i·i mod something.
- Quantities that strictly decrease: sum of values, number of inversions, potential functions ⇒ the process terminates and the number of steps is bounded.
- Permutation parity: a sequence of transpositions realises p only with the same parity as p; 3-cycles are always even.

## Construction toolbox

- **Extreme first:** place the largest/smallest element, solve the rest recursively (n → n − 1 or n → n − 2 induction). Writing the solution as a recursion on n often directly gives the code.
- **Pairing:** pair i with n + 1 − i, or adjacent pairs (1,2), (3,4), …; handle the odd leftover separately.
- **Alternating patterns:** 1, n, 2, n − 1, … (maximises adjacent differences); zig-zag for "no three monotone".
- **Sorting-based:** sort and then interleave halves (`a[0], a[n/2], a[1], a[n/2+1], …`) to avoid equal neighbours or to make every element differ from its position (derangement: rotate by one).
- **Graph constructions:** star (diameter 2), path (diameter n − 1), complete bipartite, binary tree (depth log n), cycles for "every vertex degree 2".
- **Bits:** build numbers bit by bit from the top; xor-based constructions (a, b, a ^ b); powers of two as "independent" choices (any number up to 2^k − 1 is a subset sum of 1, 2, 4, …).
- **Two operations suffice:** many "minimum operations" problems have answer ∈ {0, 1, 2}; check 0, then 1 by brute force over all single operations, else 2.
- **Answer by symmetry / mirroring** in games (second player copies the first's move on the mirror).

## Permutations and arrays

- Count/compute things via **cycles** of the permutation (swaps needed = n − cycles).
- Prefix maxima / suffix minima for "can we split the array into sorted parts".
- "Make all equal with operations on adjacent pairs": look at the **difference array** — operations become point updates.
- "Operation on a subarray": look at prefix sums — subarray operations become two point updates.
- Lexicographically smallest result: greedy left to right, choose the smallest feasible value at each position with a fast feasibility test.

## Ad-hoc reading discipline

- Many easy ACPC problems hide one special case: n = 1, all values equal, zero, negative numbers, answer exactly at a boundary, ties broken in a specific way. Before submitting, list the edge cases explicitly.
- Rephrase the problem in one mathematical sentence; if you can't, you haven't understood it yet.
- Simulation problems: follow the statement literally; don't "optimise" the rules away until a brute version works.
