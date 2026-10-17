# Greedy and Exchange Arguments

A greedy is only as good as its proof. Standard proof: take an optimal solution that differs from greedy at the first position; **swap** to agree with greedy without making it worse. If you can't prove it, stress test against a brute force (§1.2) before submitting.

## Finding the sort key by exchange

Two adjacent items i, j in an order; compare cost(…i j…) vs cost(…j i…). The difference only involves i and j ⇒ sort by that comparison.

| problem | key |
|---|---|
| minimise Σ completion times (1 machine) | ascending processing time (SPT) — ACPC 2016 *Wood Cutting* |
| minimise Σ w_i·C_i | ascending t_i / w_i, i.e. compare `t_i*w_j < t_j*w_i` (integers!) |
| minimise max lateness | ascending deadline (EDF) |
| concatenate strings to get smallest string | `a + b < b + a` |
| tasks with requirement r_i and cost c_i (need ≥ r to start, lose c) | descending r_i − c_i |
| two machines, each job on A then B (Johnson) | jobs with a<b first by a ascending, then others by b descending |

Always compare with cross-multiplication in integers instead of dividing.

## Interval greedies

- **Max number of disjoint intervals:** sort by right end, take if start > last end.
- **Min points to stab all intervals:** sort by right end; if interval not stabbed, put a point at its right end.
- **Min intervals to cover [0, L]:** sort by left; repeatedly among intervals with left ≤ covered, take the one with max right.
- **Partition intervals into min number of "rooms":** sort by start, min-heap of end times; answer = max overlap.
- **Assign intervals to k machines maximizing count:** sort by end, multiset of machine free times; assign to the machine with the latest free time ≤ start.

## Deadline scheduling

**Unit jobs with deadlines, maximise #jobs (or profit):** sort by profit desc, place each job in the latest free slot ≤ its deadline (DSU "next free slot" trick, §3.1). Or: sort by deadline, push profits into a min-heap, pop the smallest whenever heap size > deadline.

**Jobs with durations and deadlines, maximise count (Moore–Hodgson):**

```cpp
// jobs (deadline, duration); maximum number completed on time
int maxOnTime(vector<pair<ll,ll>> jobs) {
    sort(all(jobs));
    priority_queue<ll> pq; ll t = 0;
    for (auto [d, p] : jobs) {
        pq.push(p); t += p;
        if (t > d) { t -= pq.top(); pq.pop(); }   // drop the longest job so far
    }
    return sz(pq);
}
```

**Feasibility check with deadlines** (ACPC 2020 *Hired Help*: can we form k groups of K unit jobs?): a set of unit jobs is schedulable on c machines iff for every t, #jobs with deadline ≤ t is ≤ c·t. Combine with binary search on k and pick the jobs with the *latest* deadlines.

## Heap greedies

- **Merge costs (Huffman):** always merge two smallest; total cost Σ.
- **Regret / "undo" greedy:** buy low sell high with one transaction per day: min-heap; for each price p: if `top < p` then `profit += p − top`, pop, and push p (this copy lets the sale be "undone" later); then always push p. Same idea: choose k non-adjacent elements with max sum — take max, replace it with (left + right − it) node, linked list.
- **k largest sums / k best combinations:** heap of states with "next" transitions (e.g. pairs (i, j) of two sorted arrays, push (i+1, j) and (i, j+1) with a visited set).

## Other common greedy facts

- To maximise Σ a_i·b_{π(i)}: sort both same direction (rearrangement inequality); to minimise: opposite directions.
- Median minimises Σ|x − a_i|; mean minimises Σ(x − a_i)².
- Making an array non-decreasing with ±1 ops (min cost) → slope trick / median heap.
- Lexicographically smallest answer: build character by character, at each step try the smallest choice and check feasibility of completing (feasibility check is often a simple counting argument).
- Coin change greedy only works for canonical systems (1,5,10,25…); for arbitrary coins use DP.
- Pair the largest with the smallest (boats/teams with capacity): sort + two pointers.
