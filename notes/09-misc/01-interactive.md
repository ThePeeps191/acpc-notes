# Interactive Problems

ACPC 2024 had two (*Four Questions*, *Snow Way Out*). They punish protocol mistakes more than algorithmic ones.

## Protocol checklist

- **Flush after every query**: `cout << "? " << x << endl;` (`endl` flushes) or `cout.flush()`. With `printf` use `fflush(stdout)`.
- Read the judge's reply **immediately** after each query; never read ahead.
- Keep `cin.tie(nullptr)` *off* or flush manually: with `sync_with_stdio(false)` + `endl` it's fine.
- Exactly the output format: "q 43", "a 71", "3 4 ?" … including trailing tokens like `?` or `.`.
- After the final answer: **exit immediately** (`return 0`). Extra output = WA, waiting for input = TLE.
- If the judge can reply with an error (e.g. `-1`), exit at once.
- Query limits are hard: count your queries in the worst case *before* coding. "Adaptive" judges choose the hidden value to make you fail — your strategy must work for every consistent answer.

```cpp
// sketch: typical skeleton
ll ask(ll s) {
    cout << "q " << s << endl;            // endl flushes
    ll r; cin >> r;
    if (r < 0) exit(0);                   // judge error -> quit to get WA, not TLE
    return r;
}
int main() {
    // ... decide queries ...
    cout << "a " << answer << endl;
    return 0;
}
```

## Strategy patterns

- **Binary search**: hidden x in [1, N] with "higher/lower" answers ⇒ ⌈log2 N⌉ queries. Ternary answers (<, =, >) save a little.
- **Information counting**: with q queries each having k possible replies, you can distinguish at most k^q cases. If k^q < #candidates, the intended solution exploits structure (e.g. replies carry more information than they seem).
- **Number theory replies** (s² mod p for unknown prime p, *Four Questions*): the reply r = s² − kp means `p | s² − r`. Take gcd of several `s_i² − r_i` (choose s_i with s_i² > p, e.g. near the maximum allowed); strip small factors / test primality to isolate p. Use `__int128` if products exceed 64 bits.
- **Comparison-based (closer/farther/same)** on a small grid (*Snow Way Out*: 100 cells, 6 replies of 3 types): plan offline. Maintain the set of candidate cells consistent with all replies; choose the next query point that minimises the worst-case remaining set size (minimax). If greedy isn't enough, do a full game-tree search with memoisation on the candidate set (bitset of ≤ 100 cells → hash) — the search space is small. Precompute in your head / at runtime: 100 candidate cells × 100 query points × 3 replies per level.
- **Find a hidden permutation/array** with sum/xor queries: query pairs (1,2), (2,3), (1,3) to solve 3 unknowns, then chain.
- **Guess with random queries** when the judge is non-adaptive: random pivots resist bad cases.
- **Graph/tree discovery**: ask distances from one vertex, then reconstruct layer by layer.

## Testing locally

Write a tiny interactor in Python that plays the judge, and connect with a pipe:

```python
# judge.py: python3 judge.py SECRET  (talks to solution via stdin/stdout)
import sys, subprocess
secret = int(sys.argv[1])
p = subprocess.Popen(["./sol"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
queries = 0
while True:
    line = p.stdout.readline().split()
    if not line: print("solution exited without answer"); break
    if line[0] == "q":
        queries += 1
        s = int(line[1]); p.stdin.write(f"{s * s % secret}\n"); p.stdin.flush()
    else:
        print("answer", line[1], "correct" if int(line[1]) == secret else "WRONG", "queries", queries); break
```

Loop it over many secrets (including worst cases: smallest/largest values, primes near the bound).
